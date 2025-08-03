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

//���ڴ� �÷��̾�� �浹�� �� ������ ��ũ���� �����Ѵ�.
void ChestComponent::HendleCollision(Object* other_object, int chest_num)
{
	if (is_open_)
	{
		return;
	}

	is_open_ = true;

	//FMODSoundManager::Instance().PlaySound("chest", false, 0.3f);

	// ��ũ�� 1�� ����
	Object* scroll = scroll_model_->GetInstance();
	//��ũ�� ȸ���� �� ��ġ �ʱ�ȭ(���� -> ����)
	scroll->set_transform_matrix(owner_->transform_matrix() * scroll->transform_matrix());
	scroll->set_is_movable(true);
	auto scroll_comp = Object::GetComponent<ScrollComponent>(scroll);
	scroll_comp->set_is_active(true);
	scroll_comp->set_direction(XMFLOAT3(0.0f, 0.0007f, 0.0f)); // ���� �̵�

	scroll_object_ = scroll;
	scene_->AddObject(scroll);

	// �÷��̾�� ��ũ�� ����
	sc_packet_scroll_info si;
	si.type = S2C_P_SCROLL_INFO;
	si.size = sizeof(sc_packet_scroll_info);
	si.scroll_type = static_cast<int>(scroll_comp->type());
	si.chest_num = chest_num;

	std::cout << "chest_num: " << si.chest_num << std::endl;
	std::cout << "scroll_type: " << si.scroll_type << std::endl;

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
	scroll_object_->set_is_dead(true); // ��ũ�� ������Ʈ ����
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
