#pragma once
#include "CharacterComponent.h"
#include "ScrollComponent.h"

class ModelInfo;
class Scene;

class ChestComponent :
    public CharacterComponent
{
public:
    ChestComponent() = default;
    ChestComponent(Object* owner, Scene* scene = nullptr);
    ChestComponent(const std::shared_ptr<Object>& owner, Scene* scene = nullptr);
    virtual ~ChestComponent() = default;

    virtual Component* GetCopy() override;

    virtual void Update(float elapsed_time) override;

    virtual void HendleCollision(std::shared_ptr<Object> other_object) override;

    void OpenChest(uint8_t scroll_type, ModelInfo* scroll_model);

    ScrollType TakeScroll();

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

