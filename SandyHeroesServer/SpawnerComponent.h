#pragma once
#include "Component.h"
#include "Object.h"

class Scene;
class ModelInfo;

// 오브젝트에 스폰 기능을 추가하는 컴포넌트
// owner_의 위치에 생성할 컴포넌트들을 가진 오브젝트를 생성한다.
class SpawnerComponent :
	public Component
{
public:
	SpawnerComponent(Object* owner, Scene* scene);
	SpawnerComponent(Object* owner, Scene* scene, ModelInfo* model_info);

	SpawnerComponent(const SpawnerComponent& other);
	SpawnerComponent(const SpawnerComponent&& other);

	virtual Component* GetCopy() override;

	virtual void Update(float elapsed_time) override;

	// 스폰횟수, 스폰 대기시간, 스폰 쿨타임을 설정하는 함수
	void SetSpawnerInfo(int spawn_count, float spawn_time, float spawn_cool_time);
	void SetMonsterType(MonsterType type) { monster_type_ = type; }

	void ActivateSpawn(); // 스폰 시작

	void AddComponent(std::unique_ptr<Component> component);
	void AddComponent(Component* component);



	void ForceSpawn(); // 강제로 씬에 오브젝트를 스폰하는 함수 스폰 개수와 쿨타임을 무시하고 무조건 스폰한다.

private:
	std::list<std::unique_ptr<Component>> component_list_; // 스폰할 오브젝트의 컴포넌트 리스트

	ModelInfo* model_info_{ nullptr }; // 스폰할 모델 정보

	MonsterType monster_type_ = MonsterType::None;

	Scene* scene_{ nullptr }; // 스폰할 씬

	int spawn_count_{ 0 }; // 스폰 횟수
	float spawn_time_{ 0.f }; // 스폰 초기 대기 시간
	float spawn_cool_time_{ 0.f }; // 리스폰 쿨타임
	bool is_spawn_{ false }; // 스폰 시작 여부
};

