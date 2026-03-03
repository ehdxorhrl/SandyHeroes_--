#include "stdafx.h"
#include "SkinnedMeshComponent.h"
#include "Mesh.h"
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
	mesh_ = rhs.mesh_;
	mesh_->AddMeshComponent(this);
	return *this;
}

Component* SkinnedMeshComponent::GetCopy()
{
	return new SkinnedMeshComponent(*this);
}

void SkinnedMeshComponent::AttachBoneFrames(const std::vector<std::string>& bone_names)
{
	if (is_attached_bone_frames_)
		return;

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
}
