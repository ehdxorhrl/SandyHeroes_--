#include "stdafx.h"
#include "DebugMeshComponent.h"
#include "FrameResource.h"
#include "Object.h"

DebugMeshComponent::DebugMeshComponent(Object* owner, Mesh* cube_mesh, Mesh* source_mesh) 
	: MeshComponent(owner, cube_mesh), bounds_(source_mesh->bounds())
{
}

DebugMeshComponent::DebugMeshComponent(Object* owner, Mesh* cube_mesh, const BoundingBox& bounds) 
	: MeshComponent(owner, cube_mesh), bounds_(bounds)
{
}

DebugMeshComponent::DebugMeshComponent(const DebugMeshComponent& other) 
	: MeshComponent(other), bounds_(other.bounds_)
{
}

Component* DebugMeshComponent::GetCopy()
{
	return new DebugMeshComponent(*this);
}

void DebugMeshComponent::UpdateConstantBuffer(FrameResource* current_frame_resource, int cb_index)
{
	CBObject object_buffer{};
	XMMATRIX scale = XMMatrixScaling(bounds_.Extents.x, bounds_.Extents.y, bounds_.Extents.z);
	XMMATRIX translate = XMMatrixTranslation(bounds_.Center.x, bounds_.Center.y, bounds_.Center.z);
	XMMATRIX world = scale * translate * XMLoadFloat4x4(&owner_->world_matrix());

	XMStoreFloat4x4(&object_buffer.world_matrix,
		XMMatrixTranspose(world));

	UploadBuffer<CBObject>* object_cb = current_frame_resource->cb_object.get();
	object_cb->CopyData(cb_index, object_buffer);
}


