#pragma once
#include "Component.h"

class UiMeshComponent;

// UI 투명도 점점 증가시키는 컴포넌트
class FadeInUIComponent : public Component
{
public:
	FadeInUIComponent(Object* owner, float duration = 5.0f);
	FadeInUIComponent(const FadeInUIComponent& other);

	virtual Component* GetCopy() override;
	virtual void Update(float elapsed_time) override;

	// 수동으로 리셋할 수 있음
	void Reset() { elapsed_ = 0.0f; }

private:
	float duration_{ 5.0f };	// 서서히 보이는 데 걸리는 총 시간
	float elapsed_{ 0.0f };		// 누적 시간

	UiMeshComponent* ui_mesh_{ nullptr }; // 알파를 조절할 UI 컴포넌트
};