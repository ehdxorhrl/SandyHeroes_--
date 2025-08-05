#pragma once
#include "Scene.h"
#include "User.h"

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
	void UpdateObjectHitBullet();
	void UpdateObjectHitObject();
	void UpdateStageClear();

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
	void CheckObjectHitFlamethrow(Object* object, int id);


	std::list<MeshColliderComponent*> checking_maps_mesh_collider_list(int index);
	int stage_clear_num();

	void ActivateStageMonsterSpawner(int stage_num);

	void add_catch_monster_num();
	const std::list<MonsterComponent*>& monster_list() const;
private:
	//���������� ���� ������ ����Ʈ
	std::array<std::list<SpawnerComponent*>, kStageMaxCount> stage_monster_spawner_list_;

	// ���� ���� Ƚ��
	int catch_monster_num_{ 0 };
	// ���踦 ���� Ƚ��
	int get_key_num_{ 0 };

	// FŰ ����
	bool f_key_{};

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

	std::list<WallCheckObject> wall_check_object_list_;	//�� üũ�� �ʿ��� ��ü���� ����Ʈ(�÷��̾�, monster, NPC)

	std::array<std::list<GroundColliderComponent*>, 8> stage_ground_collider_list_;	//�������� �ٴ� �ݶ��̴� ����Ʈ
	std::array<std::list<WallColliderComponent*>, 8> stage_wall_collider_list_;	//�������� �� �ݶ��̴� ����Ʈ

	std::vector<Object*> dropped_guns_;

	std::vector<Object*> chests_;

	std::vector<BoxColliderComponent*> spawn_boxs_{}; // �������� ���� ���� üũ�� ���� �ڽ���

	BoundingOrientedBox stage3_clear_box_;

	bool is_prepare_ground_checking_ = false;
	//std::array<std::list<MeshColliderComponent*>, kStageMaxCount> checking_maps_mesh_collider_list_;	//�� �ٴ�üũ�� ���� �޽� �ݶ��̴� ����Ʈ �迭

};



