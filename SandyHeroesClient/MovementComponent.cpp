#include "stdafx.h"
#include "MovementComponent.h"
#include "Object.h"

MovementComponent::MovementComponent(Object* owner) : Component(owner)
{
}

MovementComponent::MovementComponent(const MovementComponent& other) : Component(other)
{
}

Component* MovementComponent::GetCopy()
{
    return new MovementComponent(*this);
}

void MovementComponent::Update(float elapsed_time)
{
    // 클라이언트에서는 물리X
}

void MovementComponent::EnableGravity()
{
    is_gravity_ = true;
}

void MovementComponent::DisableGarvity()
{
    is_gravity_ = false;

}

void MovementComponent::EnableFriction()
{
    is_friction_ = true;
}

void MovementComponent::DisableFriction()
{
    is_friction_ = false;
}

void MovementComponent::Move(float x, float y, float z, float speed)
{
    Move(XMFLOAT3{ x, y, z }, speed);
}

void MovementComponent::Move(XMFLOAT3 direction, float speed)
{
    XMFLOAT3 direction_n = xmath_util_float3::Normalize(direction);
    velocity_ += direction_n * speed;
}

void MovementComponent::MoveXZ(float x, float z, float speed)
{
    XMFLOAT3 direction{ x, 0.f, z };
    direction = xmath_util_float3::Normalize(direction);
    velocity_ += direction * speed;
}

void MovementComponent::Jump(float speed, float max_height)
{
    jump_before_y_ = owner_->position_vector().y;
    velocity_.y = speed;
    jump_max_height_ = max_height;
}

void MovementComponent::Stop()
{
    velocity_ = XMFLOAT3{ 0.f, 0.f, 0.f };
}

void MovementComponent::set_gravity_acceleration(float value)
{
    gravity_acceleration_ = value;
}

void MovementComponent::set_max_speed_xz(float value)
{
    max_speed_xz_ = value;
}

void MovementComponent::set_max_speed(float value)
{
	max_speed_ = value;
}

void MovementComponent::set_velocity(XMFLOAT3 value)
{
    velocity_ = value;
}

XMFLOAT3 MovementComponent::velocity() const
{
    return velocity_;
}

float MovementComponent::max_speed_xz() const
{
    return max_speed_xz_;
}
