#pragma once
#include "Component.h"
#include "ScrollComponent.h"

class Scene;
class ModelInfo;

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

	virtual void Update(float elapsed_time) override;

	void Heal(float amount);	//플레이어를 회복하는 함수
	void HealHp(float heal_amount);	//플레이어의 체력을 회복하는 함수
	void HealShield(float heal_amount);	//플레이어의 방패를 회복하는 함수

	void HitDamage(float damage);	//플레이어에 데미지를 입히는 함수

	void AddScroll(ScrollType type);
	bool HasScroll(ScrollType type) const;

	bool ActivateMainSkill();	//플레이어 메인 스킬 활성화 함수

	//getter
	float max_hp() const { return max_hp_; }	//플레이어 최대 체력
	float hp() const { return hp_; }		//플레이어 현재 체력
	float max_shield() const { return max_shield_; }	//플레이어 최대 방패
	float shield() const { return shield_; }	//플레이어 현재 방패
	const std::unordered_set<ScrollType>& acquired_scrolls() const;
	float main_skill_gage() const { return main_skill_gage_; }
	float main_skill_max_gage() const { return main_skill_max_gage_; }
	float dash_gage() const { return dash_gage_; }
	float dash_max_gage() const { return dash_max_gage_; }

	//setter
	void set_scene(Scene* scene) { scene_ = scene; }	//플레이어가 속한 씬 설정
	void set_dash_gage(float gage) { dash_gage_ = gage; }

private:
	Scene* scene_ = nullptr;	//플레이어가 속한 씬

	float max_hp_ = 100.f;	//플레이어 최대 체력
	float hp_ = 100.f;		//플레이어 현재 체력
	float max_shield_ = 100.f;	//플레이어 최대 방패
	float shield_ = 100.f;	//플레이어 현재 방패

	std::unordered_set<ScrollType> acquired_scrolls_;

	ModelInfo* razer_model_info_ = nullptr;	//플레이어 레이저 모델 정보
	float main_skill_gage_ = 0.f;	//플레이어 메인 스킬 게이지
	float main_skill_max_gage_ = 5.f;	//플레이어 최대 메인 스킬 게이지
	float main_skill_activation_time_ = 5.f;	//플레이어 메인 스킬 활성화 시간
	float main_skill_max_activation_time_ = 5.f;	//플레이어 최대 메인 스킬 활성화 시간
	float main_skill_razer_shot_time_ = 0.f;	//플레이어 메인 스킬 레이저 발사 후 지난 시간
	float main_skill_razer_cool_time_ = 0.3f;	//플레이어 메인 스킬 레이저 발사 간격
	float main_skill_range_ = 20.f;	//플레이어 메인 스킬 레이저 사거리

	float dash_gage_ = 0.0f;	// 대쉬 게이지
	float dash_max_gage_ = 5.0f;	// 대쉬 쿨타임 최대 시간

	float damage_cool_time_ = 1.f; // 무적시간
	float last_damage_time_ = 0.f; // 마지막으로 데미지입고 경과한 시간
	bool is_damaged = false;  // 데미지를 입었는지
};

