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
	static constexpr int kStageMaxCount{ 8 };	// 게임 스테이지 총 개수
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

	//TODO: 단일 플레이어 뿐만 아니라 적, 다른 플레이어에 대한 ground 체크도 필요
	void UpdateObjectIsGround();
	void UpdateObjectHitWall();
	void UpdateObjectHitBullet();
	void UpdateObjectHitObject();
	void UpdateStageClear();

	void PrepareGroundChecking();	//맵 바닥체크를 위한 사전 작업

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
	//스테이지별 몬스터 스포너 리스트
	std::array<std::list<SpawnerComponent*>, kStageMaxCount> stage_monster_spawner_list_;

	// 몬스터 잡은 횟수
	int catch_monster_num_{ 0 };
	// 열쇠를 먹은 횟수
	int get_key_num_{ 0 };

	// F키 누름
	bool f_key_{};

	//현재 스테이지의 스포너를 활성화 했는가?
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

	std::list<WallCheckObject> wall_check_object_list_;	//벽 체크가 필요한 객체들의 리스트(플레이어, monster, NPC)

	std::array<std::list<GroundColliderComponent*>, 8> stage_ground_collider_list_;	//스테이지 바닥 콜라이더 리스트
	std::array<std::list<WallColliderComponent*>, 8> stage_wall_collider_list_;	//스테이지 벽 콜라이더 리스트

	std::vector<Object*> dropped_guns_;

	std::vector<Object*> chests_;

	std::vector<BoxColliderComponent*> spawn_boxs_{}; // 스테이지 몬스터 생성 체크를 위한 박스들

	BoundingOrientedBox stage3_clear_box_;

	bool is_prepare_ground_checking_ = false;
	//std::array<std::list<MeshColliderComponent*>, kStageMaxCount> checking_maps_mesh_collider_list_;	//맵 바닥체크를 위한 메쉬 콜라이더 리스트 배열

};



