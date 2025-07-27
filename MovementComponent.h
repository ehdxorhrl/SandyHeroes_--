#pragma once
#include "Component.h"

//오브젝트에 이동 기능을 추가하는 컴포넌트
//속도에 따른 이동, 중력 적용 현재 2가지 기능만 구현하였다.
//단위: m/s
class MovementComponent :
    public Component
{
public:
    MovementComponent(Object* owner);
    MovementComponent(const MovementComponent& other);

    virtual Component* GetCopy() override;

    virtual void Update(float elapsed_time) override;

    void EnableGravity();
    void DisableGarvity();

    void EnableFriction();
    void DisableFriction();

    void Move(float x, float y, float z, float speed);
    void Move(XMFLOAT3 direction, float speed);

    //입력받은 xz방향으로 속력을 speed 만큼 추가한다.
    void MoveXZ(float x, float z, float speed);

    void Jump(float speed, float max_height = 999.f);

    void Stop();

    //setter
    void set_gravity_acceleration(float value);
    void set_max_speed_xz(float value);
	void set_max_speed(float value);
    //getter
    XMFLOAT3 velocity() const;

private:
    bool is_gravity_{ true };
    float gravity_acceleration_{ 20.f };

    float jump_max_height_{ 999.f };
    float jump_before_y_{};

    float max_speed_xz_{ 10.f };    //최대 속력
    float max_speed_{ 999.f };

    bool is_friction_{ true };      //마찰을 적용하는지 여부

    XMFLOAT3 velocity_{};

	XMFLOAT3 old_position_{};
};

