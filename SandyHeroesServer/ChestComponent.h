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

    void HendleCollision(std::shared_ptr<Object> other_object, int chest_num);

    ScrollType TakeScroll(int chest_num);

    void set_scroll_model(ModelInfo* scroll_model)
    {
        scroll_model_ = scroll_model;
    }

    private:
    bool is_open_{ false }; // üƮ ִ 
    ModelInfo* scroll_model_{ nullptr };

    std::weak_ptr<Object> scroll_object_{}; // ũ Ʈ

    Scene* scene_{ nullptr }; // �� ����
};

