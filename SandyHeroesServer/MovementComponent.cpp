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
    old_position_ = owner_->position_vector();

    if (owner_->is_player() && velocity_.y > 0)
    {
        
        std::cout << "[Session ID: " << "] "
            << owner_->name() << "elapsed_time" << elapsed_time << std::endl;
    }

    if (is_gravity_)
    {
        velocity_.y -= gravity_acceleration_ * elapsed_time;
    }

    //지면에 서있으면 더 이상 떨어지지 않음.
    if (owner_->is_ground() && velocity_.y < 0.f)
    {
        velocity_.y = 0.f;
    }

    //점프 높이 제한
    if (velocity_.y > 0 && (jump_before_y_ + jump_max_height_) <= owner_->position_vector().y)
    {
        velocity_.y = 0.f;
    }

    //최대 속력 제한
    float speed = xmath_util_float3::Length(velocity_);
    XMFLOAT3 direction = xmath_util_float3::Normalize(velocity_);
    if (speed > max_speed_)
    {
        velocity_ = xmath_util_float3::ScalarProduct(direction, max_speed_);
    }

    //xz 평면 이동에 필요한 변수들
    XMFLOAT3 velocity_xz{ velocity_.x, 0.f, velocity_.z };
    XMFLOAT3 direction_xz = xmath_util_float3::Normalize(velocity_xz);
    float speed_xz = xmath_util_float3::Length(velocity_xz);
    constexpr float kFrictionAcceleration{ 70.f };

    //xz 평면 최대 속도 제한
    if (speed_xz > max_speed_xz_)
    {
        velocity_.x *= (max_speed_xz_ / speed_xz);
        velocity_.z *= (max_speed_xz_ / speed_xz);
    }

    //마찰 구현
    if (is_friction_)
    {
        if (is_gravity_)
        {
            if (speed_xz < elapsed_time * kFrictionAcceleration)
            {
                velocity_ = { 0, velocity_.y, 0 };
            }
            else
            {
                velocity_ -= direction_xz * elapsed_time * kFrictionAcceleration;
            }
        }
        else
        {
            if (speed < elapsed_time * kFrictionAcceleration)
            {
                velocity_ = { 0, 0, 0 };
            }
            else
            {
                velocity_ -= direction * elapsed_time * kFrictionAcceleration;
            }
        }
    }

    owner_->set_position_vector(owner_->position_vector() + (velocity_ * (elapsed_time +0.00015)));

    

    //if (owner_->is_player() && velocity_.y > 0)
    //{
    //    std::cout << "[Session ID: " << "] "
    //        << owner_->name() << "점프 후 y값" << owner_->position_vector().y << std::endl;
    //    std::cout << "점프 후 velocity_.y값" << velocity_.y << std::endl;
    //}
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

XMFLOAT3 MovementComponent::velocity() const
{
    return velocity_;
}

float MovementComponent::max_speed_xz() const
{
    return max_speed_xz_;
}

void MovementComponent::BackupPosition()
{
    old_position_ = owner_->position_vector();
}
