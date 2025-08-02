#include "stdafx.h"
#include "ChestComponent.h"
#include "AnimatorComponent.h"
#include "Object.h"
#include "MovementComponent.h"
#include "ScrollComponent.h"
#include "Scene.h"
//#include "FMODSoundManager.h"

ChestComponent::ChestComponent(Object* owner, Scene* scene)
	: Component(owner),is_open_(false), scene_(scene)
{
}

Component* ChestComponent::GetCopy()
{
	return new ChestComponent(*this);
}

void ChestComponent::Update(float elapsed_time)
{
}

//상자는 플레이어와 충돌할 때 열리고 스크롤을 생성한다.
void ChestComponent::HendleCollision(Object* other_object)
{
	if (is_open_)
	{
		return;
	}

	is_open_ = true;

	//FMODSoundManager::Instance().PlaySound("chest", false, 0.3f);

	// 스크롤 1개 생성
	Object* scroll = scroll_model_->GetInstance();
	//스크롤 회전값 및 위치 초기화(월드 -> 상자)
	scroll->set_transform_matrix(owner_->transform_matrix() * scroll->transform_matrix());
	scroll->set_is_movable(true);
	auto scroll_comp = Object::GetComponent<ScrollComponent>(scroll);
	scroll_comp->set_is_active(true);
	scroll_comp->set_direction(XMFLOAT3(0.0f, 0.0007f, 0.0f)); // 위로 이동

	scroll_object_ = scroll;
	scene_->AddObject(scroll);

	// 플레이어에게 스크롤 전송

	std::cout << "박스 충돌 완료" << std::endl;

	//auto scroll_comp = Object::GetComponent<ScrollComponent>(scroll);
	//scroll_comp->set_direction(XMFLOAT3(0.000f, 0.0007f, 0.f));
	//scroll_comp->set_type(ScrollType::)
	//scroll->AddComponent(scroll_comp);

}

ScrollType ChestComponent::TakeScroll()
{
	if (!scroll_object_)
	{
		return ScrollType::None;
	}
	auto scroll_comp = Object::GetComponent<ScrollComponent>(scroll_object_);
	auto type = scroll_comp->type();
	scroll_object_->set_is_dead(true); // 스크롤 오브젝트 제거
	scroll_object_ = nullptr;

	//FMODSoundManager::Instance().PlaySound("scroll_pickup", true, 0.3f);

	//animator_->animation_state()->ChangeAnimationTrack(
	//	(int)ChestAnimationTrack::kOpenToClose, owner_, animator_);

	return type;
}
