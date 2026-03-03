#include "stdafx.h"
#include "BillboardMesh.h"
#include "Object.h"
#include "FrameResource.h"
#include "GameFramework.h"
#include "Scene.h"
#include "CameraComponent.h"
#include "MeshComponent.h"

void BillboardMesh::UpdateConstantBuffer(FrameResource* curr_frame_resource, int& start_index)
{
	instance_count_ = 0;
	instance_buffer_offset_ = curr_frame_resource->current_instance_offset;

	const auto& camera_object = GameFramework::Instance()->scene()->main_camera()->owner();
	if (!camera_object)
	{
		OutputDebugString(L"BillboardMeshComponent::UpdateConstantBuffer: Camera object is null.\n");
	}

	for (const auto& comp : mesh_component_list_)
	{
		auto lock_comp = comp.lock();
		if(!lock_comp)
			continue;
		if (!lock_comp->IsVisible())
			continue;
		const auto& object = lock_comp->owner();

		XMVECTOR pos = XMLoadFloat3(&object->world_position_vector());
		XMVECTOR camPos = XMLoadFloat3(&camera_object->world_position_vector());
		XMVECTOR up = XMLoadFloat3(&object->world_up_vector());

		// Ä«¸Ş¶ó¿Í ºôº¸µå »çÀÌ º¤ÅÍ
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
			XMMatrixTranspose(world_matrix));

		data.time = object->life_time();


		curr_frame_resource->sb_instance_data->CopyData(
			curr_frame_resource->current_instance_offset++,
			data
		);

		instance_count_++;
	}

}

void BillboardMesh::UpdateConstantBufferForShadow(FrameResource* curr_frame_resource, int& start_index)
{
	instance_count_ = 0;
	instance_buffer_offset_ = curr_frame_resource->current_instance_offset;

	const auto& camera_object = GameFramework::Instance()->scene()->main_camera()->owner();
	if (!camera_object)
	{
		OutputDebugString(L"BillboardMeshComponent::UpdateConstantBuffer: Camera object is null.\n");
	}

	for (const auto& comp : mesh_component_list_)
	{
		auto lock_comp = comp.lock();
		if(!lock_comp)
			continue;
		const auto& object = lock_comp->owner();

		XMVECTOR pos = XMLoadFloat3(&object->world_position_vector());
		XMVECTOR camPos = XMLoadFloat3(&camera_object->world_position_vector());
		XMVECTOR up = XMLoadFloat3(&object->world_up_vector());

		// Ä«¸Ş¶ó¿Í ºôº¸µå »çÀÌ º¤ÅÍ
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
			XMMatrixTranspose(world_matrix));

		data.time = object->life_time();

		curr_frame_resource->sb_instance_data->CopyData(
			curr_frame_resource->current_instance_offset++,
			data
		);

		instance_count_++;
	}

}
