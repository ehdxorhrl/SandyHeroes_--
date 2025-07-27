#pragma once
#include "Component.h"
class BoxColliderComponent :
    public Component
{
public:
	BoxColliderComponent(Object* owner, const BoundingBox& box);
	BoxColliderComponent(Object* owner, const BoundingOrientedBox& box);

	BoxColliderComponent(const BoxColliderComponent& other);

	virtual Component* GetCopy() override;

	virtual void Update(float elapsed_time) override;

	void set_box(const BoundingOrientedBox& box);

	const BoundingOrientedBox& box() const;
	BoundingOrientedBox animated_box() const;

private:
	BoundingOrientedBox box_{};
	BoundingOrientedBox animated_box_{};
};

