#pragma once
#include "Component.h"
#include "BaseScene.h"

class AIComponent;
struct Node;

//enum class MonsterType { kNormal, kBoss, kMiniBoss };
enum class StatusEffectType { None, Fire, Poison, Electric };
struct StatusEffect
{
	//StatusEffectType  type = StatusEffectType::None;
	float duration = 0.f;
	float elapsed = 0.f;
	float fire_damage = 0.0f;

	bool flame_frenzy = false;
	bool acid_frenzy = false;
	bool electric_frenzy = false;

	bool IsActive() const { return elapsed < duration; }
};

class MonsterComponent :
	public Component
{
public:
	DECLARE_COMPONENT(MonsterComponent, Component)

	MonsterComponent(Object* owner);
	MonsterComponent(const std::shared_ptr<Object>& owner);
	MonsterComponent(const MonsterComponent& other);
	virtual ~MonsterComponent();

	virtual Component* GetCopy() override;

	virtual void Update(float elapsed_time) override;

	void UpdateTargetPath();

	void InitAfterOwnerSet();

	void HitDamage(float damage); //���Ϳ� �������� ����

	void ApplyStatusEffect(StatusEffectType type, float duration, float damage,
		bool flame_frenzy, bool acid_frenzy, bool electric_frenzy);

	//setter
	void set_max_shield(float value);
	void set_shield(float value);
	void set_max_hp(float value);
	void set_hp(float value);
	void set_attack_force(float value);
	void set_target(std::shared_ptr<Object> target); 
	void set_is_pushed(bool is_pushed);
	void set_push_timer(float value);
	void set_scene(Scene* value);

	void RebuildBehaviorTree_();

	//getter
	float shield() const;
	float hp()const;
	float max_hp()const;
	float max_shield()const;
	float attack_force()const;
	std::shared_ptr<Object> target() const;
	Scene* scene() const;

	bool IsDead() const;

private:
	float max_hp_{ 90.f };
	float hp_{ 90.f };
	float shield_{ 50.f };
	float max_shield_{ 50.f };
	float attack_force_{};

	std::weak_ptr<Object> target_;

	bool is_dead_animationing_{ false };	

	Scene* scene_{ nullptr };

	
	bool is_pushed_{ false };
	float push_timer_{ 0.0f }; 

	bool electric_slow_applied_{ false };
	float original_speed_{ 3.5 };

	bool dead_by_fire_{ false };

	std::unordered_map<StatusEffectType, StatusEffect> status_effects_;

	std::shared_ptr<AIComponent> ai_ = nullptr;
	int current_node_idx_{ 0 }; 
	Node* current_node_{ nullptr }; 
	float astar_delta_cool_time_{ 0.0f }; 
	std::vector<Node*> path_; 

};

