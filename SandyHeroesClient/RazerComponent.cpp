#include "stdafx.h"
#include "RazerComponent.h"
#include "Object.h"
#include "FMODSoundManager.h"

RazerComponent::RazerComponent(Object* owner) : Component(owner)
{
}

RazerComponent::RazerComponent(Object* owner, XMFLOAT3 start, XMFLOAT3 end)
{
    owner_ = owner;
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
        owner_->set_is_dead(true); // 레이저가 죽으면 오브젝트를 제거한다.
        return;
    }
}

void RazerComponent::InitRazer(XMFLOAT3 start, XMFLOAT3 end)
{
    start_position_ = start;
    end_position_ = end;
    owner_->set_position_vector(start);
    XMFLOAT3 direction = { end.x - start.x, end.y - start.y, end.z - start.z };
    direction = xmath_util_float3::Normalize(direction);
    owner_->set_up_vector(direction);
    owner_->set_right_vector(xmath_util_float3::Normalize(xmath_util_float3::CrossProduct(owner_->up_vector(), owner_->look_vector())));
    owner_->set_look_vector(xmath_util_float3::Normalize(xmath_util_float3::CrossProduct(owner_->right_vector(), owner_->up_vector())));
}
