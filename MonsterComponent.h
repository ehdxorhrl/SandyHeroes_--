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

	//setter
	void set_shield(float value);
	void set_hp(float value);
	void set_attack_force(float value);
	void set_target(Object* target); //몬스터가 공격할 타겟 설정

	//getter
	float shield() const;
	float hp()const;
	float attack_force()const;

	void set_scene(Scene* value);
	
	bool IsDead() const;

private:
	float max_hp_{ 120.f };

	float shield_{};
	float hp_{ 120.f };
	float attack_force_{};

	MonsterType monster_type_{ MonsterType::kNormal };

	Object* target_{ nullptr };	//몬스터가 공격할 타겟

	bool is_dead_animationing_{ false };	//죽는 애니메이션이 재생중인지 여부

	//TODO: 몬스터를 움직일 AI 추가
};

