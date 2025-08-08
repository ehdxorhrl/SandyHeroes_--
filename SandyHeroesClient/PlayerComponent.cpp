#include "stdafx.h"
#include "PlayerComponent.h"
#include "ScrollComponent.h"
#include "FMODSoundManager.h"
#include "Scene.h"
#include "RazerComponent.h"
#include "BaseScene.h"
#include "MonsterComponent.h"
#include "ModelInfo.h"

PlayerComponent::PlayerComponent(Object* owner)
	: Component(owner)
{
}

PlayerComponent::PlayerComponent(Object* owner, float max_hp, float hp, float max_shield, float shield)
	: Component(owner), max_hp_(max_hp), hp_(hp), max_shield_(max_shield), shield_(shield)
{
}

Component* PlayerComponent::GetCopy()
{
    return new PlayerComponent(*this);
}

void PlayerComponent::Update(float elapsed_time)
{
	if (main_skill_activation_time_ < main_skill_max_activation_time_)
	{
		main_skill_activation_time_ += elapsed_time;
		main_skill_razer_shot_time_ += elapsed_time;
		if (main_skill_razer_cool_time_ < main_skill_razer_shot_time_) {
			main_skill_razer_shot_time_ = 0.f; // 레이저 발사 후 지난 시간을 초기화
			auto base_scene = dynamic_cast<BaseScene*>(scene_);
			if (!base_scene) return;
			auto monster_list = base_scene->monster_list();
			for (auto& monster : monster_list)
			{
				float length = xmath_util_float3::Length(
					monster->owner()->world_position_vector() - owner_->world_position_vector());
				if (length <= main_skill_range_)
				{
					XMFLOAT3 monster_pos = monster->owner()->world_position_vector();
					std::uniform_real_distribution<float> random_position(-5.f, 5.f);
					XMFLOAT3 start_pos = { monster_pos.x,
										   monster_pos.y,
										   monster_pos.z };
					XMFLOAT3 end_pos = { monster_pos.x + random_position(kRandomGenerator),
										 monster_pos.y + 50.f,
										 monster_pos.z + random_position(kRandomGenerator) };

					auto razer = razer_model_info_->GetInstance();
					auto razer_component = Object::GetComponent<RazerComponent>(razer);
					razer_component->InitRazer(start_pos, end_pos);
					scene_->AddObject(razer);
				}
			}
		}
	}
}

void PlayerComponent::Heal(float amount)
{
	if (amount <= 0.f)
	{
		return; // 회복량이 0 이하인 경우 아무 작업도 하지 않음
	}
	float rest{};
	if (hp_ < max_hp_)
	{
		hp_ += amount;
		if (hp_ > max_hp_)
		{
			rest = hp_ - max_hp_; // 최대 체력을 넘는 부분을 rest에 저장
			hp_ = max_hp_; // hp가 최대 체력을 넘지 않도록
		}
	}
	else
	{
		rest = amount; // hp가 이미 최대 체력인 경우, rest에 전체 회복량 저장
		hp_ = max_hp_; // 이미 최대 체력인 경우
	}

	if (rest < 0.f)
		return;

	if(shield_ < max_shield_)
	{
		shield_ += rest; // 남은 회복량을 방패에 추가
		if (shield_ > max_shield_)
		{
			shield_ = max_shield_; // shield가 최대 방패를 넘지 않도록
		}
	}
	else
	{
		shield_ = max_shield_; // 이미 최대 방패인 경우
	}
}

void PlayerComponent::HealHp(float heal_amount)
{
	if (hp_ < max_hp_)
	{
		hp_ += heal_amount;
		if (hp_ > max_hp_)
		{
			hp_ = max_hp_; // hp가 최대 체력을 넘지 않도록
		}
	}
	else
	{
		hp_ = max_hp_; // 이미 최대 체력인 경우
	}
}

void PlayerComponent::HealShield(float heal_amount)
{
	if (shield_ < max_shield_)
	{
		shield_ += heal_amount;
		if (shield_ > max_shield_)
		{
			shield_ = max_shield_; // shield가 최대 방패를 넘지 않도록
		}
	}
	else
	{
		shield_ = max_shield_; // 이미 최대 방패인 경우
	}
}

void PlayerComponent::HitDamage(float damage)
{
	if (shield_ > 0.f)
	{
		shield_ -= damage;
		if (shield_ < 0.f)
		{
			hp_ += shield_; // shield_가 음수면 hp에 더해짐
			shield_ = 0.f; // shield는 0으로 설정
		}
	}
	else
	{
		hp_ -= damage;
	}
	if (hp_ < 0.f)
	{
		hp_ = 0.f; // hp가 음수로 내려가지 않도록
	}
}

void PlayerComponent::AddScroll(ScrollType type)
{
	if (type == ScrollType::None) return;

	acquired_scrolls_.insert(type);
}

bool PlayerComponent::HasScroll(ScrollType type) const
{
	return acquired_scrolls_.count(type) > 0;
}

void PlayerComponent::ActivateMainSkill()
{
	if (!razer_model_info_)
	{
		razer_model_info_ = scene_->FindModelInfo("Razer");
	}

	main_skill_gage_ = 0.f;
	main_skill_activation_time_ = 0.f;


}

const std::unordered_set<ScrollType>& PlayerComponent::acquired_scrolls() const
{
	return acquired_scrolls_;
}
