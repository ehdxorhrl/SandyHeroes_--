#include "stdafx.h"
#include "RazerComponent.h"
#include "Object.h"
#include "FMODSoundManager.h"
#include "GameFramework.h"
#include "Scene.h"

RazerComponent::RazerComponent(Object* owner) : Component(owner)
{
}
RazerComponent::RazerComponent(const std::shared_ptr<Object>& owner) : Component(owner)
{
}

RazerComponent::RazerComponent(Object* owner, XMFLOAT3 start, XMFLOAT3 end) 
{
    owner_ = owner->shared_from_this();
    InitRazer(start, end);
}
RazerComponent::RazerComponent(const std::shared_ptr<Object>& owner, XMFLOAT3 start, XMFLOAT3 end) 
{
    owner_ = owner->shared_from_this();
    InitRazer(start, end);
}

RazerComponent::RazerComponent(const RazerComponent& other) : Component(other)
{
}

Component* RazerComponent::GetCopy()
{
    return new RazerComponent(*this);
}

void RazerComponent::Update(float elapsed_time)
{
    life_time_ += elapsed_time;

    if (life_time_ / max_life_time_ > 0.8f)
    {
        if (!is_collision_active_)
        {
            FMODSoundManager::Instance().PlaySound("lazer", false, 0.025f);
        }
        is_collision_active_ = true;
    }

    if (life_time_ > max_life_time_)
    {
        if (auto locked_owner = owner_.lock())
        {
            GameFramework::Instance()->scene()->DeleteObject(locked_owner);
        }
        return;
    }
}

void RazerComponent::InitRazer(XMFLOAT3 start, XMFLOAT3 end)
{
    start_position_ = start;
    end_position_ = end;
    auto locked_owner = owner_.lock();
    if (!locked_owner) 
        return;
    XMVECTOR direction = { end.x - start.x, end.y - start.y, end.z - start.z };
    direction = XMVector3NormalizeEst(direction);
	auto& rotation_q = GetRotationBetweenVectors(XMVECTOR{ 0, 1, 0 }, direction);
	XMMATRIX rotation_matrix = XMMatrixRotationQuaternion(rotation_q);
	auto& transform_matrix = XMLoadFloat4x4(&locked_owner->transform_matrix()) * rotation_matrix;
    XMFLOAT4X4 new_transform_matrix{};
	XMStoreFloat4x4(&new_transform_matrix, transform_matrix);
	locked_owner->set_transform_matrix(new_transform_matrix);
    locked_owner->set_position_vector(start);
}
