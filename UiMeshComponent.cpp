#include "stdafx.h"
#include "UiMeshComponent.h"
#include "FrameResource.h"
#include "Scene.h"
#include "CameraComponent.h"
#include "UIMesh.h"

UiMeshComponent::UiMeshComponent(Object* owner, Mesh* mesh) 
    : MeshComponent(owner, mesh)
{
	name_ = mesh->name();
}

UiMeshComponent::UiMeshComponent(Object* owner, Mesh* mesh, Material* material, Scene* scene)
    : MeshComponent(owner, mesh, material), scene_(scene)
{
	name_ = mesh->name();

}

UiMeshComponent::UiMeshComponent(const UiMeshComponent& other)
    : MeshComponent(other), scene_(other.scene_)
{
	name_ = other.mesh_->name();

}

UiMeshComponent& UiMeshComponent::operator=(const UiMeshComponent& rhs)
{
	owner_ = nullptr;
	mesh_ = rhs.mesh_;
	mesh_->AddMeshComponent(this);
	return *this;
}

Component* UiMeshComponent::GetCopy()
{
    return new UiMeshComponent(*this);
}

void UiMeshComponent::UpdateConstantBuffer(FrameResource* current_frame_resource, int cb_index)
{
	if (!is_visible_)
		return;

	const auto& camera = scene_->main_camera();
	const auto& ui_mesh = static_cast<UIMesh*>(mesh_);
	XMFLOAT2 screen_pos{};
	CBUi ui_info{};
	if (!is_static_)
	{
		XMVECTOR world_pos = XMLoadFloat3(&owner_->world_position_vector());
		XMMATRIX view = XMLoadFloat4x4(&camera->view_matrix());
		XMMATRIX proj = XMLoadFloat4x4(&camera->projection_matrix());

		XMVECTOR clip_pos = XMVector3TransformCoord(world_pos, view * proj);

		// Z < 0 인 경우 카메라 뒤에 있으므로 렌더 안 함
		if (clip_pos.m128_f32[2] < 0.0f || clip_pos.m128_f32[2] > 1.0f)
		{
			is_in_screen_ = false;
			return;
		}
		else
		{
			is_in_screen_ = true;
		}

		screen_pos = { (clip_pos.m128_f32[0] + 1.0f) * 0.5f * scene_->screen_size().x,
			(1.0f - clip_pos.m128_f32[1]) * 0.5f * scene_->screen_size().y };
		screen_pos.x -= ui_mesh->ui_size().x / 2.f; //pivot is left-top
		screen_pos.y -= ui_mesh->ui_size().y / 2.f;
	}
	else
	{
		screen_pos = ui_mesh->screen_position();
		is_in_screen_ = true;
	}
	ui_info.screen_offset = screen_pos;
	ui_info.width_ratio = ui_ratio_.x;
	ui_info.height_ratio = ui_ratio_.y;

	constant_buffer_index_ = cb_index;

	UploadBuffer<CBUi>* cb_ui = current_frame_resource->cb_ui.get();
	cb_ui->CopyData(cb_index, ui_info);

}

void UiMeshComponent::Render(Material* material, ID3D12GraphicsCommandList* command_list, FrameResource* curr_frame_resource)
{
	if (!is_visible_)
		return;
	if (!is_in_screen_)
		return;
	auto gpu_address = curr_frame_resource->cb_ui->Resource()->GetGPUVirtualAddress();
	const auto cb_size = d3d_util::CalculateConstantBufferSize((sizeof(CBUi)));
	gpu_address += cb_size * constant_buffer_index_;

	command_list->SetGraphicsRootConstantBufferView((int)RootParameterIndex::kUi, gpu_address);

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

std::string UiMeshComponent::name() const
{
	return name_;
}

void UiMeshComponent::set_name(const std::string& value)
{
	name_ = value;
}

void UiMeshComponent::set_is_static(bool value)
{
	is_static_ = value;
}

void UiMeshComponent::set_ui_ratio(XMFLOAT2 value)
{
	ui_ratio_ = value;
}
