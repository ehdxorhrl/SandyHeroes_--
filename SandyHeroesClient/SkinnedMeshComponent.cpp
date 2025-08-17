#include "stdafx.h"
#include "SkinnedMeshComponent.h"
#include "Mesh.h"
#include "FrameResource.h"
#include "Object.h"

SkinnedMeshComponent::SkinnedMeshComponent(Object* owner, Mesh* mesh) 
	: MeshComponent(owner, mesh)
{
}

SkinnedMeshComponent::SkinnedMeshComponent(const SkinnedMeshComponent& other) : MeshComponent(other)
{
}

SkinnedMeshComponent& SkinnedMeshComponent::operator=(const SkinnedMeshComponent& rhs)
{
	owner_ = nullptr;
	mesh_ = rhs.mesh_;
	mesh_->AddMeshComponent(this);
	return *this;
}

Component* SkinnedMeshComponent::GetCopy()
{
    return new SkinnedMeshComponent(*this);
}

void SkinnedMeshComponent::UpdateConstantBuffer(FrameResource* current_frame_resource, int cb_index)
{
	//if (!is_visible_)
	//	return;

	if (cb_index == -1) cb_index = constant_buffer_index_;
	constant_buffer_index_ = cb_index;

	CBBoneTransform bone_transform_buffer{};

	for (int i = 0; i < bone_frames_.size(); ++i)
	{
		XMStoreFloat4x4(&bone_transform_buffer.bone_transform_matrix[i],
			XMMatrixTranspose(XMLoadFloat4x4(&bone_frames_[i]->world_matrix())));
	}
	UploadBuffer<CBBoneTransform>* bone_transform_cb = current_frame_resource->cb_bone_transform.get();
	bone_transform_cb->CopyData(cb_index, bone_transform_buffer);

}

void SkinnedMeshComponent::UpdateConstantBufferForShadow(FrameResource* current_frame_resource, int cb_index)
{

	if (cb_index == -1) cb_index = constant_buffer_index_;
	constant_buffer_index_ = cb_index;

	CBBoneTransform bone_transform_buffer{};

	for (int i = 0; i < bone_frames_.size(); ++i)
	{
		XMStoreFloat4x4(&bone_transform_buffer.bone_transform_matrix[i],
			XMMatrixTranspose(XMLoadFloat4x4(&bone_frames_[i]->world_matrix())));
	}
	UploadBuffer<CBBoneTransform>* bone_transform_cb = current_frame_resource->cb_bone_transform.get();
	bone_transform_cb->CopyData(cb_index, bone_transform_buffer);

}

void SkinnedMeshComponent::Render(Material* material, ID3D12GraphicsCommandList* command_list, FrameResource* curr_frame_resource)
{
	if (!is_visible_)
		return;

	auto gpu_address = curr_frame_resource->cb_bone_transform->Resource()->GetGPUVirtualAddress();
	const auto cb_size = d3d_util::CalculateConstantBufferSize((sizeof(CBBoneTransform)));
	gpu_address += cb_size * constant_buffer_index_;

	command_list->SetGraphicsRootConstantBufferView((int)RootParameterIndex::kBoneTransform, gpu_address);

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

void SkinnedMeshComponent::AttachBoneFrames(const std::vector<std::string>& bone_names)
{
	if (is_attached_bone_frames_)
		return;

	bone_frames_.clear();
	bone_frames_.reserve(bone_names.size());

	Object* hierarchy_root = owner_->GetHierarchyRoot();

	for (const std::string& name : bone_names)
	{
		Object* bone_frame = hierarchy_root->FindFrame(name);
		bone_frames_.push_back(bone_frame);
	}

	is_attached_bone_frames_ = true;

	for (int i = 0; i < bone_frames_.size(); ++i)
	{
		if (bone_frames_[i] == nullptr)
		{
			is_attached_bone_frames_ = false;
		}
	}
}
