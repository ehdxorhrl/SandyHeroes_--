#pragma once
#include "MeshColliderComponent.h"
class GroundColliderComponent :
    public MeshColliderComponent
{
public:
    GroundColliderComponent(Object* owner) : MeshColliderComponent(owner) {}
    GroundColliderComponent(const GroundColliderComponent& other) : MeshColliderComponent(other) {}

    virtual Component* GetCopy() override
    {
        return new GroundColliderComponent(*this);
    }
private:

};

