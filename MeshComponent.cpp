#include "stdafx.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "Object.h"
#include "FrameResource.h"
#include "DescriptorManager.h"

MeshComponent::MeshComponent(Object* owner, Mesh* mesh) : Component(owner), mesh_(mesh)
{
	mesh->AddMeshComponent(this);
}

MeshComponent::MeshComponent(const MeshComponent& other) : Component(other), mesh_(other.mesh_)
{
	other.mesh_->AddMeshComponent(this);
}

MeshComponent& MeshComponent::operator=(const MeshComponent& rhs)
{
	//컴포넌트 복제시 owner_는 리셋되어야함
	owner_ = nullptr;
	mesh_ = rhs.mesh_;
	mesh_->AddMeshComponent(this);
	return *this;
}

Component* MeshComponent::GetCopy()
{
	return new MeshComponent(*this);
}

void MeshComponent::UpdateConstantBuffer(FrameResource* current_frame_resource, int cb_index)
{
	CBObject object_buffer{};
	XMStoreFloat4x4(&object_buffer.world_matrix,
		XMMatrixTranspose(XMLoadFloat4x4(&owner_->world_matrix())));

	UploadBuffer<CBObject>* object_cb = current_frame_resource->cb_object.get();
	object_cb->CopyData(cb_index, object_buffer);
}

bool MeshComponent::IsVisible() const
{
	return is_visible_;
}

Mesh* MeshComponent::GetMesh() const
{
	return mesh_;
}
