#pragma once
#include "Component.h"
class PlayerComponent :
    public Component
{
public:
	PlayerComponent() {};
	PlayerComponent(Object* owner);
	PlayerComponent(Object* owner, float max_hp, float hp, float max_shield, float shield);
	PlayerComponent(const PlayerComponent& other) = default; //복사 생성자
	virtual ~PlayerComponent() {}

	virtual Component* GetCopy() override;

	void Heal(float amount);	//플레이어를 회복하는 함수
	void HealHp(float heal_amount);	//플레이어의 체력을 회복하는 함수
	void HealShield(float heal_amount);	//플레이어의 방패를 회복하는 함수

	void HitDamage(float damage);	//플레이어에 데미지를 입히는 함수

	//getter
	float max_hp() const { return max_hp_; }	//플레이어 최대 체력
	float hp() const { return hp_; }		//플레이어 현재 체력
	float max_shield() const { return max_shield_; }	//플레이어 최대 방패
	float shield() const { return shield_; }	//플레이어 현재 방패

private:
	float max_hp_ = 100.f;	//플레이어 최대 체력
	float hp_ = 100.f;		//플레이어 현재 체력
	float max_shield_ = 100.f;	//플레이어 최대 방패
	float shield_ = 100.f;	//플레이어 현재 방패

};

