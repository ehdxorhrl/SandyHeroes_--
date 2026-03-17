#include "stdafx.h"
#include "SkinnedMeshComponent.h"
#include "Mesh.h"
#include "FrameResource.h"
#include "Object.h"
#include "SkinnedMesh.h"

SkinnedMeshComponent::SkinnedMeshComponent(Object* owner, Mesh* mesh) 
	: MeshComponent(owner, mesh)
{
	mesh_type_ = MeshType::kSkinnedMesh;
}
SkinnedMeshComponent::SkinnedMeshComponent(const std::shared_ptr<Object>& owner, Mesh* mesh) 
	: MeshComponent(owner, mesh)
{
	mesh_type_ = MeshType::kSkinnedMesh;

}

SkinnedMeshComponent::SkinnedMeshComponent(const SkinnedMeshComponent& other) : MeshComponent(other)
{
	mesh_type_ = MeshType::kSkinnedMesh;

}

SkinnedMeshComponent& SkinnedMeshComponent::operator=(const SkinnedMeshComponent& rhs)
{
	mesh_ = rhs.mesh_;
	return *this;
}

Component* SkinnedMeshComponent::GetCopy()
{
    return new SkinnedMeshComponent(*this);
}

void SkinnedMeshComponent::UpdateConstantBuffer(FrameResource* current_frame_resource)
{
	AttachBoneFrames();

	constant_buffer_index_ = current_frame_resource->current_bone_transform_offset;

	CBBoneTransform bone_transform_buffer{};

	for (int i = 0; i < bone_frames_.size(); ++i)
	{
		XMStoreFloat4x4(&bone_transform_buffer.bone_transform_matrix[i],
			XMMatrixTranspose(XMLoadFloat4x4(&bone_frames_[i]->world_matrix())));
	}
	UploadBuffer<CBBoneTransform>* bone_transform_cb = current_frame_resource->cb_bone_transform.get();
	bone_transform_cb->CopyData(current_frame_resource->current_bone_transform_offset++, bone_transform_buffer);

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

	mesh_->Render(command_list, material_index, curr_frame_resource);
}

void SkinnedMeshComponent::AttachBoneFrames()
{
	if (is_attached_bone_frames_)
		return;

	const auto& bone_names = static_cast<SkinnedMesh*>(mesh_)->bone_names();

	bone_frames_.clear();
	bone_frames_.reserve(bone_names.size());

	auto locked_owner = owner_.lock();
	if (!locked_owner) return;
	std::shared_ptr<Object> hierarchy_root = locked_owner->GetHierarchyRoot();

	for (const std::string& name : bone_names)
	{
		std::shared_ptr<Object> bone_frame = hierarchy_root->FindFrame(name);
		bone_frames_.push_back(bone_frame.get());
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
