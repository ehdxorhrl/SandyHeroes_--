#pragma once
#include "Component.h"

enum class UiType
{
	kButton,
	kImage,
	kSliderX,		// 가로 슬라이더
	kSliderY,		// 세로 슬라이더
	kProgressBarX,	// 가로 진행률 표시줄
	kProgressBarY,	// 세로 진행률 표시줄
	kPanel,
	kText,			// 텍스트
	kCustom
};

class UiComponent :
    public Component
{
public:
	UiComponent() {};
	UiComponent(Object* owner);
	UiComponent(const std::shared_ptr<Object>& owner);
	UiComponent(Object* owner, UiType type);
	UiComponent(const std::shared_ptr<Object>& owner, UiType type);
	UiComponent(const UiComponent& other);
	virtual ~UiComponent() {}

	virtual Component* GetCopy();
	virtual void Update(float elapsed_time) {}

	void set_type(UiType type) { type_ = type; }
	void set_view(std::shared_ptr<Object> view) { view_ = view; }

protected:
	UiType type_{ UiType::kImage }; // UI 타입
	std::weak_ptr<Object> view_{}; // ui가 특정 오브젝트를 주시하기 위한 포인터

};

