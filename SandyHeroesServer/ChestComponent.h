#pragma once
#include "ScrollComponent.h"

class ModelInfo;
class Scene;

class ChestComponent :
    public Component
{
public:
    ChestComponent() = default;
    ChestComponent(Object* owner, Scene* scene = nullptr);
    virtual ~ChestComponent() = default;

    virtual Component* GetCopy() override;

    virtual void Update(float elapsed_time) override;

    void HendleCollision(Object* other_object);

    ScrollType TakeScroll();

    void set_scroll_model(ModelInfo* scroll_model)
    {
        scroll_model_ = scroll_model;
    }

private:
    bool is_open_{ false }; // 체스트가 열려있는지 여부
    ModelInfo* scroll_model_{ nullptr };

    Object* scroll_object_{ nullptr }; // 스크롤 오브젝트

    Scene* scene_{ nullptr }; // 씬 정보
};

