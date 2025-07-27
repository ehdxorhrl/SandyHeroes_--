#include "stdafx.h"
#include "ColliderComponent.h"
#include "Object.h"

ColliderComponent::ColliderComponent(Object* owner, BoundingOrientedBox* bounding_box) : Component(owner)
{
	bounding_box_ = bounding_box;
}

ColliderComponent::ColliderComponent(const ColliderComponent& other) : Component(nullptr)
{

}

Component* ColliderComponent::GetCopy()
{
	return new ColliderComponent(*this);
}

void ColliderComponent::Update(float elapsed_time)
{
	bounding_box_->Transform(animated_bounding_box_, XMLoadFloat4x4(&owner_->world_matrix()));
	XMStoreFloat4(&animated_bounding_box_.Orientation, XMQuaternionNormalize(XMLoadFloat4(&animated_bounding_box_.Orientation)));
}

bool ColliderComponent::CheckOBBMeshCollision(const MeshComponent* other_mesh, const XMFLOAT4X4& player_world_matrix, const XMFLOAT4X4& other_world_matrix)
{
	if (nullptr == other_mesh) return false; // 메쉬 데이터가 없는 경우

	auto indices = other_mesh->GetMesh()->indices_array();
	auto positions = other_mesh->GetMesh()->positions();

	for (int i = 0; i < indices.size(); ++i)
	{
		for (int j = 0; j < indices[i].size(); ++j) 
		{
			XMVECTOR v0 = XMVector3TransformCoord(XMLoadFloat3(&positions[j]), XMLoadFloat4x4(&other_world_matrix));
			XMVECTOR v1 = XMVector3TransformCoord(XMLoadFloat3(&positions[j + 1]), XMLoadFloat4x4(&other_world_matrix));
			XMVECTOR v2 = XMVector3TransformCoord(XMLoadFloat3(&positions[j + 2]), XMLoadFloat4x4(&other_world_matrix));

			if (animated_bounding_box_.Intersects(v0, v1, v2))
			{
				return true;
			}
		}
	}

	return false; // 충돌 없음
}

bool ColliderComponent::CheckRayGroundCollision(const MeshComponent* other_mesh, const XMFLOAT3& rayOrigin, const XMFLOAT3& rayDir, float maxDistance, const XMFLOAT4X4& worldMatrix)
{
	XMMATRIX matWorld = XMLoadFloat4x4(&worldMatrix);

	auto& indices = other_mesh->GetMesh()->indices_array();
	auto& positions = other_mesh->GetMesh()->positions();

	for (UINT i = 0; i < indices.size(); i += 3)
	{
		XMVECTOR v0 = XMVector3TransformCoord(XMLoadFloat3(&positions[i]), matWorld);
		XMVECTOR v1 = XMVector3TransformCoord(XMLoadFloat3(&positions[i+1]), matWorld);
		XMVECTOR v2 = XMVector3TransformCoord(XMLoadFloat3(&positions[i+2]), matWorld);

		float distance = 0.0f;
		if (RayIntersectsTriangle(XMLoadFloat3(&rayOrigin), XMLoadFloat3(&rayDir), v0, v1, v2, distance))
		{
			if (distance <= maxDistance)
				return true;
		}
	}
	return false;
}

bool ColliderComponent::RayIntersectsTriangle(XMVECTOR rayOrigin, XMVECTOR rayDir, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, float& t)
{
	constexpr float EPSILON = 0.000001f;
	XMVECTOR edge1 = XMVectorSubtract(v1, v0);
	XMVECTOR edge2 = XMVectorSubtract(v2, v0);
	XMVECTOR h = XMVector3Cross(rayDir, edge2);
	float a = XMVectorGetX(XMVector3Dot(edge1, h));

	if (fabs(a) < EPSILON)
		return false; // 평행

	float f = 1.0f / a;
	XMVECTOR s = XMVectorSubtract(rayOrigin, v0);
	float u = f * XMVectorGetX(XMVector3Dot(s, h));
	if (u < 0.0f || u > 1.0f)
		return false;

	XMVECTOR q = XMVector3Cross(s, edge1);
	float v = f * XMVectorGetX(XMVector3Dot(rayDir, q));
	if (v < 0.0f || u + v > 1.0f)
		return false;

	t = f * XMVectorGetX(XMVector3Dot(edge2, q));
	if (t > EPSILON)
		return true;

	return false;
}

bool ColliderComponent::Intersects(ColliderComponent* collider_object)
{
	return (animated_bounding_box_.Intersects(collider_object->animated_bounding_box_));
}

XMFLOAT3 ColliderComponent::extent() const
{
	return bounding_box_->Extents;
}
