#pragma once
#include "Scene.h"
#include "User.h"
#include "RazerComponent.h"

class MonsterComponent;
class MeshColliderComponent;
class SpawnerComponent;
class BoxColliderComponent;
class GroundColliderComponent;
class WallColliderComponent;
class MovementComponent;
class Session;

class BaseScene :
	public Scene
{
private:
	static constexpr int kStageMaxCount{ 8 };	// ���� �������� �� ����
public:
	virtual void BuildMesh() override;

	virtual void BuildModelInfo();

	virtual void BuildObject() override;

	void CreateMonsterSpawner();

	virtual bool ProcessInput(void* p) override;

	virtual void Update(float elapsed_time) override;

	Object* CreateAndRegisterPlayer(long long session_id);

	void AddObject(Object* object) override;
	void DeleteObject(Object* object);
	void DeleteDeadObjects();

	//TODO: ���� �÷��̾� �Ӹ� �ƴ϶� ��, �ٸ� �÷��̾ ���� ground üũ�� �ʿ�
	void UpdateObjectIsGround();
	void UpdateObjectHitWall();
	void UpdateObjectHitBullet(float elapsed_time);
	void UpdateObjectHitObject();
	void UpdateStageClear();
	void UpdateRazerHitEnemy();

	void PrepareGroundChecking();	//�� �ٴ�üũ�� ���� ���� �۾�

	void CheckPlayerHitGun(Object* object);
	void CheckObjectIsGround(Object* object);
	void CheckPlayerHitWall(Object* object, MovementComponent* movement);
	void CheckObjectHitObject(Object* object);
	void CheckObjectHitBullet(Object* object);
	void CheckPlayerHitPyramid(Object* object);
	void CheckSpawnBoxHitPlayers();
	void CheckPlayerHitChest();
	void CheckRayHitEnemy(const XMFLOAT3& ray_origin, const XMFLOAT3& ray_direction, int id);
	void CheckObjectHitFlamethrow(Object* object, int id, float elapsed_time);

	void CheckRazerHitEnemy(RazerComponent* razer_component, MonsterComponent* monster_component);

	void TickNoClipTimers(float elapsed_time); // Update()���� ȣ��


	std::list<MeshColliderComponent*> checking_maps_mesh_collider_list(int index);
	std::list<WallColliderComponent*> stage_wall_collider_list(int index);
	int stage_clear_num();

	bool InRangeXZ(Object* self, Object* target, float r);

	void ActivateStageMonsterSpawner(int stage_num);

	void add_catch_monster_num();
	void add_stage_clear_num() { ++stage_clear_num_; }
	const std::list<MonsterComponent*>& monster_list() const;

	void set_is_activate_spawner(bool is_activate) { is_activate_spawner_ = is_activate; }

private:
	//���������� ���� ������ ����Ʈ
	std::array<std::list<SpawnerComponent*>, kStageMaxCount> stage_monster_spawner_list_;

	// ���� ���� Ƚ��
	int catch_monster_num_{ 0 };
	// ���踦 ���� Ƚ��
	int get_key_num_{ 0 };

	// FŰ ����
	bool f_key_{};

	bool is_game_clear_{ false };	//���� Ŭ���� ����

	//���� ���������� �����ʸ� Ȱ��ȭ �ߴ°�?
	bool is_activate_spawner_ = false;

	struct WallCheckObject
	{
		Object* object{ nullptr };
		MovementComponent* movement{ nullptr };

		WallCheckObject(Object* obj, MovementComponent* move)
			: object(obj), movement(move) {
		}
	};
	std::list<MonsterComponent*> monster_list_;

	std::list<RazerComponent*> razer_list_;	//������ ����Ʈ

	std::list<WallCheckObject> wall_check_object_list_;	//�� üũ�� �ʿ��� ��ü���� ����Ʈ(�÷��̾�, monster, NPC)

	std::array<std::list<GroundColliderComponent*>, 8> stage_ground_collider_list_;	//�������� �ٴ� �ݶ��̴� ����Ʈ
	std::array<std::list<WallColliderComponent*>, 8> stage_wall_collider_list_;	//�������� �� �ݶ��̴� ����Ʈ

	std::unordered_map<uint64_t, float> noclip_monmon_;

	std::vector<Object*> dropped_guns_;

	std::vector<Object*> chests_;

	std::vector<BoxColliderComponent*> spawn_boxs_{}; // �������� ���� ���� üũ�� ���� �ڽ���

	BoundingOrientedBox stage3_clear_box_;

	// Ʃ��
	float noclip_monmon_radius_ = 1.0f;   // ����-���� ���� �Ǵ� �ݰ�(XZ)
	float noclip_monmon_duration_ = 1.0f;  // �浹 ���� ���� �ð�

	bool is_prepare_ground_checking_ = false;
	//std::array<std::list<MeshColliderComponent*>, kStageMaxCount> checking_maps_mesh_collider_list_;	//�� �ٴ�üũ�� ���� �޽� �ݶ��̴� ����Ʈ �迭

	// �� Ű: (minId<<32) | maxId  ���·� ��ĪŰ ����
	static inline uint64_t PairKey_(int a, int b) {
		uint32_t x = (uint32_t)std::min(a, b);
		uint32_t y = (uint32_t)std::max(a, b);
		return (uint64_t(x) << 32) | uint64_t(y);
	}

};



