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

    void set_camera_object(std::shared_ptr<Object> value);
    void set_scene(Scene* value);
    void set_particle(std::shared_ptr<ParticleComponent> value);

    //getter
    bool is_firekey_down() const;

    std::shared_ptr<Object> camera_object() const { return camera_object_.lock(); }

private:
    Scene* scene_ = nullptr;
    std::weak_ptr<Object> camera_object_;

    std::weak_ptr<ParticleComponent> particle_;

    bool is_firekey_down_ = false;

    // ����Ű�� ���Ȱ� owner�� �������� �ִٸ� ������ ����
    bool is_jumpkey_pressed_ = false;
    float jump_speed_ = 7.f;

    float y_axis_velocity_ = 0.f;       // y�࿡ ���� �ӵ�
    float gravity_ = 20.f;

    bool is_dash_pressed_ = false;
    float dash_cool_delta_time_ = 0.f;          // �뽬 ��Ÿ�� ������ ���� ����
    float dash_cool_time_ = 0.f;                // �뽬 ��Ÿ��
    XMFLOAT3 dash_velocity_{ 0,0,0 };           // �뽬 �ӵ�(���� ����Ǵ� "�ӵ�"��)
    float dash_length_{ 10.f };                  // �뽬 �Ÿ�
    XMFLOAT3 dash_before_position_{ 0,0,0 };    // �뽬 ������ġ
};

