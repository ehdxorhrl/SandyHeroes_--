#pragma once
#include "Component.h"
#include "MeshComponent.h"

class ColliderComponent : public Component
{
public:
	ColliderComponent() = delete;
	ColliderComponent(Object* owner, BoundingOrientedBox* bounding_box);
	ColliderComponent(const ColliderComponent& other);
	virtual ~ColliderComponent() {}

	virtual Component* GetCopy() override;

	virtual void Update(float elapsed_time) override;

	bool CheckOBBMeshCollision(const MeshComponent* other_mesh, const XMFLOAT4X4& player_world_matrix, const XMFLOAT4X4& other_world_matrix);

	bool CheckRayGroundCollision(const MeshComponent* other_mesh, const XMFLOAT3& rayOrigin, const XMFLOAT3& rayDir, float maxDistance, const XMFLOAT4X4& worldMatrix);
	bool RayIntersectsTriangle(XMVECTOR rayOrigin, XMVECTOR rayDir, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, float& t);

	bool Intersects(ColliderComponent* collider_object);

	//getter
	XMFLOAT3 extent() const;
protected:
	BoundingOrientedBox* bounding_box_ = nullptr;
	BoundingOrientedBox animated_bounding_box_{};
};