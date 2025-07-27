#include "stdafx.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "Object.h"
#include "FrameResource.h"
#include "DescriptorManager.h"
#include "Material.h"
#include "Scene.h"

MeshComponent::MeshComponent(Object* owner, Mesh* mesh) : Component(owner), mesh_(mesh)
{
	mesh->AddMeshComponent(this);
}

MeshComponent::MeshComponent(Object* owner, Mesh* mesh, Material* material) : Component(owner), mesh_(mesh)
{
	mesh->AddMeshComponent(this);
	AddMaterial(material);
}

MeshComponent::MeshComponent(const MeshComponent& other) : Component(other), mesh_(other.mesh_)
{
	other.mesh_->AddMeshComponent(this);
	materials_.reserve(other.materials_.size());
	for (const auto& const material : other.materials_)
	{
		AddMaterial(material);
	}
}

MeshComponent& MeshComponent::operator=(const MeshComponent& rhs)
{
	//컴포넌트 복제시 owner_는 리셋되어야함
	owner_ = nullptr;
	mesh_ = rhs.mesh_;
	mesh_->AddMeshComponent(this);
	return *this;
}

MeshComponent::~MeshComponent()
{
	if (mesh_)
	{
		mesh_->DeleteMeshComponent(this);
	}

	for (const auto& const material : materials_)
	{
		material->DeleteMeshComponent(this);
	}
}

Component* MeshComponent::GetCopy()
{
	return new MeshComponent(*this);
}

void MeshComponent::UpdateConstantBuffer(FrameResource* current_frame_resource, int cb_index)
{
	if (!is_visible_)
		return;

	if (cb_index == -1) cb_index = constant_buffer_index_;
	constant_buffer_index_ = cb_index;

	CBObject object_buffer{};
	XMStoreFloat4x4(&object_buffer.world_matrix,
		XMMatrixTranspose(XMLoadFloat4x4(&owner_->world_matrix())));

	UploadBuffer<CBObject>* object_cb = current_frame_resource->cb_object.get();
	object_cb->CopyData(cb_index, object_buffer);
}

void MeshComponent::UpdateConstantBufferForShadow(FrameResource* current_frame_resource, int cb_index)
{
	if (cb_index == -1) cb_index = constant_buffer_index_;
	constant_buffer_index_ = cb_index;

	CBObject object_buffer{};
	XMStoreFloat4x4(&object_buffer.world_matrix,
		XMMatrixTranspose(XMLoadFloat4x4(&owner_->world_matrix())));

	UploadBuffer<CBObject>* object_cb = current_frame_resource->cb_object.get();
	object_cb->CopyData(cb_index, object_buffer);
}

void MeshComponent::AddMaterial(Material* material)
{
	materials_.push_back(material);
	material->AddMeshComponent(this);
}

bool MeshComponent::ChangeMaterial(int index, Material* material)
{
	if (materials_.size() < index)
	{
		return false;
	}

	materials_[index]->DeleteMeshComponent(this);
	materials_[index] = material;
	material->AddMeshComponent(this);

	return true;
}

void MeshComponent::Render(Material* material, ID3D12GraphicsCommandList* command_list, 
	FrameResource* curr_frame_resource)
{
	if (!is_visible_)
		return;
	auto gpu_address = curr_frame_resource->cb_object->Resource()->GetGPUVirtualAddress();
	const auto cb_size = d3d_util::CalculateConstantBufferSize((sizeof(CBObject)));
	gpu_address += cb_size * constant_buffer_index_;

	command_list->SetGraphicsRootConstantBufferView((int)RootParameterIndex::kWorldMatrix, gpu_address);

	int material_index{};
	for (int i = 0; i < materials_.size(); ++i)
	{
		if (material == materials_[i])
		{
			material_index = i;
			break;
		}
	}

	mesh_->Render(command_list, material_index);
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
	is_in_view_frustum_ = value;
}

bool MeshComponent::is_in_view_frustum() const
{
	return is_in_view_frustum_;
}

Mesh* MeshComponent::GetMesh() const
{
	return mesh_;
}

void MeshComponent::set_mesh(Mesh* mesh)
{
	mesh_ = mesh;
}

UINT MeshComponent::constant_buffer_index() const
{
	return constant_buffer_index_;
}

