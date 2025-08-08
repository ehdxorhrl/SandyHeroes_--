#include "stdafx.h"
#include "BillboardMeshComponent.h"
#include "FrameResource.h"
#include "Object.h"
#include "Scene.h"
#include "CameraComponent.h"

BillboardMeshComponent::BillboardMeshComponent(Object* owner, Mesh* mesh) : MeshComponent(owner, mesh)
{
}

BillboardMeshComponent::BillboardMeshComponent(Object* owner, Mesh* mesh, Material* material)
	: MeshComponent(owner, mesh, material)
{
}

BillboardMeshComponent::BillboardMeshComponent(Object* owner, Mesh* mesh, Material* material, Scene* scene)
	: MeshComponent(owner, mesh, material), scene_(scene)
{
}

BillboardMeshComponent::BillboardMeshComponent(const BillboardMeshComponent& other) : MeshComponent(other), scene_(other.scene_)
{
}

Component* BillboardMeshComponent::GetCopy()
{
	return new BillboardMeshComponent(*this);
}

void BillboardMeshComponent::UpdateConstantBuffer(FrameResource* current_frame_resource, int cb_index)
{
	if (!is_visible_)
		return;

	if (cb_index == -1) cb_index = constant_buffer_index_;
	constant_buffer_index_ = cb_index;

	auto camera_object = scene_->main_camera()->owner();
	if (!camera_object)
	{
		OutputDebugString(L"BillboardMeshComponent::UpdateConstantBuffer: Camera object is null.\n");
	}

	XMVECTOR pos = XMLoadFloat3(&owner_->world_position_vector());
	XMVECTOR camPos = XMLoadFloat3(&camera_object->world_position_vector());
	XMVECTOR up = XMLoadFloat3(&owner_->world_up_vector());

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

	XMMATRIX world_matrix = XMLoadFloat4x4(&owner_->world_matrix());

	XMVECTOR scale;
	XMVECTOR dummy_rot;
	XMVECTOR dummy_pos;
	XMMatrixDecompose(&scale, &dummy_rot, &dummy_pos, world_matrix);

	XMMATRIX scale_matrix = XMMatrixScalingFromVector(scale);
	XMMATRIX translation = XMMatrixTranslationFromVector(pos);

	world_matrix = XMMatrixTranspose(scale_matrix * rotation * translation);
	CBObject object_buffer{};
	XMStoreFloat4x4(&object_buffer.world_matrix,
		world_matrix);
	object_buffer.time = owner_->life_time();
	UploadBuffer<CBObject>* object_cb = current_frame_resource->cb_object.get();
	object_cb->CopyData(cb_index, object_buffer);

}

