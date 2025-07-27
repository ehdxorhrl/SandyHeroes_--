#pragma once
#include "MeshColliderComponent.h"

class BoxColliderComponent;

class WallColliderComponent :
    public MeshColliderComponent
{
public:
    WallColliderComponent(Object* owner) : MeshColliderComponent(owner) {}
    WallColliderComponent(const WallColliderComponent& other) : MeshColliderComponent(other) {}

    virtual Component* GetCopy() override
    {
        return new WallColliderComponent(*this);
    }

    virtual bool CollisionCheckByRay(FXMVECTOR ray_origin, FXMVECTOR ray_direction, float& out_distance) override;

    void set_box_collider(BoxColliderComponent* box_collider)
    {
        box_collider_ = box_collider;
    }

private:
    BoxColliderComponent* box_collider_ = nullptr; // 벽 콜라이더를 위한 박스 콜라이더

};

