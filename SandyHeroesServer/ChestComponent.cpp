#include "stdafx.h"
#include "ChestComponent.h"
#include "AnimatorComponent.h"
#include "Object.h"
#include "MovementComponent.h"
#include "ScrollComponent.h"
#include "Scene.h"
#include "SessionManager.h"
#include "User.h"
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
void ChestComponent::HendleCollision(Object* other_object, int chest_num)
{
	if (is_open_) return;
	is_open_ = true;

	// 스크롤 1개 생성
	Object* scroll = scroll_model_->GetInstance();

	//스크롤 회전값 및 위치 초기화(월드 -> 상자)
	scroll->set_transform_matrix(owner_->transform_matrix() * scroll->transform_matrix());
	scroll->set_is_movable(true);

	if (auto* scroll_comp = Object::GetComponent<ScrollComponent>(scroll)) {
		if (auto* proto_comp =
			Object::GetComponent<ScrollComponent>(scroll_model_->hierarchy_root())) {
			scroll_comp->set_type(proto_comp->type()); // ← 여기 추가
		}
		scroll_comp->set_is_active(true);
		scroll_comp->set_direction(XMFLOAT3(0.0f, 0.0007f, 0.0f));
	}

	scroll_object_ = scroll;
	scene_->AddObject(scroll);

	// 플레이어에게 스크롤 전송
	sc_packet_scroll_info si;
	si.type = S2C_P_SCROLL_INFO;
	si.size = sizeof(sc_packet_scroll_info);
	si.scroll_type = static_cast<int>(Object::GetComponent<ScrollComponent>(scroll)->type());
	si.chest_num = chest_num;

	std::cout << "chest_num: " << static_cast<int>(si.chest_num) << '\n';
	std::cout << "scroll_type: " << static_cast<int>(si.scroll_type) << '\n';

	const auto& users = SessionManager::getInstance().getAllSessions();
	for (auto& u : users) {
		u.second->do_send(&si);
	}

}

ScrollType ChestComponent::TakeScroll(int chest_num)
{
	if (!scroll_object_)
	{
		return ScrollType::None;
	}
	auto scroll_comp = Object::GetComponent<ScrollComponent>(scroll_object_);
	auto type = scroll_comp->type();
	std::cout << "스크롤타입: " << static_cast<int>(type) << std::endl;
	scroll_object_->set_is_dead(true); // 스크롤 오브젝트 제거
	scroll_object_ = nullptr;

	//FMODSoundManager::Instance().PlaySound("scroll_pickup", true, 0.3f);

	//animator_->animation_state()->ChangeAnimationTrack(
	//	(int)ChestAnimationTrack::kOpenToClose, owner_, animator_);

	sc_packet_take_scroll tc;
	tc.type = S2C_P_TAKE_SCROLL;
	tc.size = sizeof(sc_packet_take_scroll);
	tc.chest_num = chest_num;

	const auto& users = SessionManager::getInstance().getAllSessions();
	for (auto& u : users) {
		u.second->do_send(&tc);
	}

	return type;
}
