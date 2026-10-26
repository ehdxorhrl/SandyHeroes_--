#include "stdafx.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "Object.h"
#include "FrameResource.h"
#include "DescriptorManager.h"
#include "Material.h"
#include "Scene.h"
#include "GameFramework.h"
#include "CameraComponent.h"

MeshComponent::MeshComponent(Object* owner, Mesh* mesh) : Component(owner), mesh_(mesh)
{
}
MeshComponent::MeshComponent(const std::shared_ptr<Object>& owner, Mesh* mesh) : Component(owner), mesh_(mesh)
{
}

MeshComponent::MeshComponent(Object* owner, Mesh* mesh, Material* material) : Component(owner), mesh_(mesh)
{
	AddMaterial(material);
}
MeshComponent::MeshComponent(const std::shared_ptr<Object>& owner, Mesh* mesh, Material* material) : Component(owner), mesh_(mesh)
{
	AddMaterial(material);
}

MeshComponent::MeshComponent(const MeshComponent& other) : Component(other), 
mesh_(other.mesh_),
is_visible_(other.is_visible_),
is_in_view_frustum_(other.is_in_view_frustum_),
is_in_shadow_map_obb_(other.is_in_shadow_map_obb_),
is_using_view_frustum_culling(other.is_using_view_frustum_culling),
is_using_shadow_map_obb_culling(other.is_using_shadow_map_obb_culling)
{
	materials_.reserve(other.materials_.size());
	for (const auto& const material : other.materials_)
	{
		AddMaterial(material);
	}
}

MeshComponent& MeshComponent::operator=(const MeshComponent& rhs)
{
	//컴포넌트 복제시 owner_는 리셋되어야함
	mesh_ = rhs.mesh_;
	return *this;
}

MeshComponent::~MeshComponent()
{
}

Component* MeshComponent::GetCopy()
{
	return new MeshComponent(*this);
}

void MeshComponent::Update(float elapsed_time)
{
	for(int i = 0; i < materials_.size(); ++i)
	{
		materials_[i]->AddMeshComponent(std::static_pointer_cast<MeshComponent>(shared_from_this()));
	}

	is_in_view_frustum_ = !is_using_view_frustum_culling;
	is_in_shadow_map_obb_ = !is_using_shadow_map_obb_culling;
}

void MeshComponent::UpdateConstantBuffer(FrameResource* current_frame_resource)
{
	//weak_ptr이므로 lock해서 shared_ptr로 변환 후 사용
	const auto& object = owner_.lock();
	if (!object) return;
	
	//기존 상수 버퍼 인덱스를 인스턴싱 적용에 따라 인스턴스 버퍼의 오프셋으로 활용
	constant_buffer_index_ = current_frame_resource->current_instance_offset;

	InstanceData data{};
	XMStoreFloat4x4(&data.world_matrix,
		XMMatrixTranspose(XMLoadFloat4x4(&object->world_matrix())));

	data.time = object->life_time();

	if(is_in_view_frustum_)
	{
		current_frame_resource->sb_main_visible_indices->CopyData(
			current_frame_resource->current_main_visible_offset++,
			constant_buffer_index_
		);
	}

	if(is_in_shadow_map_obb_)
	{
		current_frame_resource->sb_shadow_visible_indices->CopyData(
			current_frame_resource->current_shadow_visible_offset++,
			constant_buffer_index_
		);
	}

	current_frame_resource->sb_instance_data->CopyData(
		current_frame_resource->current_instance_offset++,
		data
	);

}

void MeshComponent::UpdateConstantBufferForBillboard(FrameResource* current_frame_resource)
{
	const auto& camera_object = GameFramework::Instance()->scene()->main_camera()->owner();
	if(!camera_object)
	{
		OutputDebugString(L"MeshComponent::UpdateConstantBufferForBillboard: Camera object is null.\n");
		return;
	}

	const auto& object = owner_.lock();
	if(!object)
		return;

	constant_buffer_index_ = current_frame_resource->current_instance_offset;

	XMVECTOR pos = XMLoadFloat3(&object->world_position_vector());
	XMVECTOR camPos = XMLoadFloat3(&camera_object->world_position_vector());
	XMVECTOR up = XMLoadFloat3(&object->world_up_vector());

	// 카메라와 빌보드 사이 벡터
	XMVECTOR look = XMVector3Normalize(camPos - pos);
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(up, look));
	up = XMVector3Cross(look, right);

	XMMATRIX rotation = {
		right,
		up,
		look,
		XMVectorSet(0, 0, 0, 1)
	};

	XMMATRIX world_matrix = XMLoadFloat4x4(&object->world_matrix());

	XMVECTOR scale;
	XMVECTOR dummy_rot;
	XMVECTOR dummy_pos;
	XMMatrixDecompose(&scale, &dummy_rot, &dummy_pos, world_matrix);

	XMMATRIX scale_matrix = XMMatrixScalingFromVector(scale);
	XMMATRIX translation = XMMatrixTranslationFromVector(pos);

	world_matrix = XMMatrixTranspose(scale_matrix * rotation * translation);

	InstanceData data{};
	XMStoreFloat4x4(&data.world_matrix,
		world_matrix);

	data.time = object->life_time();

	if (is_in_view_frustum_)
	{
		current_frame_resource->sb_main_visible_indices->CopyData(
			current_frame_resource->current_main_visible_offset++,
			constant_buffer_index_
		);
	}

	if (is_in_shadow_map_obb_)
	{
		current_frame_resource->sb_shadow_visible_indices->CopyData(
			current_frame_resource->current_shadow_visible_offset++,
			constant_buffer_index_
		);
	}

	current_frame_resource->sb_instance_data->CopyData(
		current_frame_resource->current_instance_offset++,
		data
	);
}

void MeshComponent::AddMaterial(Material* material)
{
	materials_.push_back(material);
}

bool MeshComponent::ChangeMaterial(int index, Material* material)
{
	if (materials_.size() < index)
	{
		return false;
	}

	materials_[index] = material;

	return true;
}

void MeshComponent::Render(Material* material, ID3D12GraphicsCommandList* command_list, 
	FrameResource* curr_frame_resource)
{
	if (!is_visible_)
		return;
	// material과 서브메쉬 인덱스 매칭
	int material_index{};
	for (int i = 0; i < materials_.size(); ++i)
	{
		if (material == materials_[i])
		{
			material_index = i;
			break;
		}
	}

	mesh_->Render(command_list, material_index, curr_frame_resource);
}

bool MeshComponent::IsVisible() const
{
	return is_visible_;
}

void MeshComponent::set_is_visible(bool value)
{
	is_visible_ = value;
}

void MeshComponent::set_is_in_view_frustum(bool value)
{
	if (is_using_view_frustum_culling)
		is_in_view_frustum_ = value;
}

bool MeshComponent::is_in_view_frustum() const
{
	if(is_using_view_frustum_culling)
		return is_in_view_frustum_;
	return true;
}

Mesh* MeshComponent::GetMesh() const
{
	return mesh_;
}

Material* MeshComponent::GetMaterial(int index) const
{
	return materials_[index];
}


void MeshComponent::set_mesh(Mesh* mesh)
{
	mesh_ = mesh;
}

UINT MeshComponent::constant_buffer_index() const
{
	return constant_buffer_index_;
}

int MeshComponent::GetMaterialIndex(Material* material) const
{
	for (int i = 0; i < materials_.size(); ++i)
	{
		if (materials_[i] == material)
			return i;
	}
	return -1;
}

