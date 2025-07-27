#pragma once
#include "Component.h"
#include "BaseScene.h"

enum class MonsterType { kNormal, kBoss, kMiniBoss };

//Object를 몬스터의 기능을 추가하는 컴포넌트
class MonsterComponent :
    public Component
{
public:
	MonsterComponent(Object* owner);
	MonsterComponent(const MonsterComponent& other);
	virtual ~MonsterComponent() {}

	virtual Component* GetCopy() override;

	virtual void Update(float elapsed_time) override;

	void HitDamage(float damage); //몬스터에 데미지를 입힘

	//setter
	void set_shield(float value);
	void set_hp(float value);
	void set_attack_force(float value);
	void set_target(Object* target); //몬스터가 공격할 타겟 설정
	void set_is_pushed(bool is_pushed);
	void set_push_timer(float value);

	//getter
	float shield() const;
	float hp()const;
	float max_hp()const;
	float max_shield()const;
	float attack_force()const;

	void set_scene(Scene* value);
	
	bool IsDead() const;

private:
	float max_hp_{ 90.f };

	float shield_{ 50.f };
	float max_shield_{ 50.f };	
	float hp_{ 90.f };
	float attack_force_{};

	MonsterType monster_type_{ MonsterType::kNormal };

	Object* target_{ nullptr };	//몬스터가 공격할 타겟

	bool is_dead_animationing_{ false };	//죽는 애니메이션이 재생중인지 여부
	
	// 몬스터 겹침 현상 없애기 위한 변수
	bool is_pushed_{ false };
	float push_timer_{ 0.0f }; // 밀린 후 일정 시간 이동 금지

	//TODO: 몬스터를 움직일 AI 추가
};

