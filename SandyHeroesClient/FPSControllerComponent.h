#pragma once
#include "InputControllerComponent.h"
#include "ParticleComponent.h"

class Scene;

class FPSControllerComponent :
    public InputControllerComponent
{
public:
    FPSControllerComponent(Object* owner);
    virtual ~FPSControllerComponent() {}

    virtual Component* GetCopy();

    virtual bool ProcessInput(UINT message_id, WPARAM w_param, LPARAM l_param, float message_time) override;

    virtual void Update(float elapsed_time) override;

    void Stop();

    void set_camera_object(Object* value);
    void set_scene(Scene* value);
    void set_particle(ParticleComponent* value);

    //getter
    bool is_firekey_down() const;

    Object* camera_object() const { return camera_object_; }

private:
    Scene* scene_ = nullptr;
    Object* camera_object_ = nullptr;

    ParticleComponent* particle_ = nullptr;

    bool is_firekey_down_ = false;

    // 점프키가 눌렸고 owner가 지면위에 있다면 점프를 실행
    bool is_jumpkey_pressed_ = false;
    float jump_speed_ = 7.f;

    float y_axis_velocity_ = 0.f;       // y축에 대한 속도
    float gravity_ = 20.f;

    bool is_dash_pressed_ = false;
    float dash_cool_delta_time_ = 0.f;          // 대쉬 쿨타임 측정을 위한 변수
    float dash_cool_time_ = 0.f;                // 대쉬 쿨타임
    XMFLOAT3 dash_velocity_{ 0,0,0 };           // 대쉬 속도(실제 적용되는 "속도"임)
    float dash_length_{ 10.f };                  // 대쉬 거리
    XMFLOAT3 dash_before_position_{ 0,0,0 };    // 대쉬 시작위치
};

