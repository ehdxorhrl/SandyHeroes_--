#include "stdafx.h"
#include "FadeInUIComponent.h"
#include "UiMeshComponent.h"
#include "Object.h"

FadeInUIComponent::FadeInUIComponent(Object* owner, float duration)
	: Component(owner), duration_(duration)
{
	ui_mesh_ = Object::GetComponent<UiMeshComponent>(owner_);
	if (ui_mesh_)
	{
		ui_mesh_->set_alpha(0.0f); // 처음은 투명
		ui_mesh_->set_is_visible(true); // 혹시라도 비활성화돼 있으면 켜기
	}
}

FadeInUIComponent::FadeInUIComponent(const FadeInUIComponent& other)
	: Component(other), duration_(other.duration_), elapsed_(other.elapsed_)
{
	// 복사 생성자에서는 owner_가 아직 없기 때문에 Init은 따로 해줘야 함
}

Component* FadeInUIComponent::GetCopy()
{
	return new FadeInUIComponent(*this);
}

void FadeInUIComponent::Update(float elapsed_time)
{
	if (!ui_mesh_)
	{
		ui_mesh_ = Object::GetComponent<UiMeshComponent>(owner_);
		if (!ui_mesh_) return;
	}

	elapsed_ += elapsed_time;
	float alpha = std::min(1.0f, elapsed_ / duration_);
	ui_mesh_->set_alpha(alpha);
}