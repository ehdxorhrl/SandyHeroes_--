#include "PlayerComponent.h"

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
