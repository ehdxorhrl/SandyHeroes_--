#pragma once
#include "Scene.h"
#include "ParticleSystem.h"
#include "CutSceneTrack.h"
#include "TextRenderer.h"
#include "TextFormat.h"
#include "RazerComponent.h"

class MeshColliderComponent;
class SpawnerComponent;
class BoxColliderComponent;
class GroundColliderComponent;
class WallColliderComponent;
class MovementComponent;
class MonsterComponent;

class BaseScene :
    public Scene
{
private:
	static constexpr int kStageMaxCount{ 8 };	// 게임 스테이지 총 개수
public:
	virtual void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* command_list,
		ID3D12RootSignature* root_signature, GameFramework* game_framework, ID2D1DeviceContext* device_context, IDWriteFactory* dwrite_factory) override;
	virtual void BuildShader(ID3D12Device* device, ID3D12RootSignature* root_signature) override;
	virtual void BuildMesh(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;
	virtual void BuildMaterial(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;
	virtual void BuildObject(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;
	virtual void BuildTextBrushAndFormat(ID2D1DeviceContext* device_context, IDWriteFactory* dwrite_factory) override;
	void BuildModelInfo(ID3D12Device* device); // 오브젝트 재사용을 위해 모델 정보를 새로 만들거나 기존 모델 정보를 수정하는 함수

	void CreatePlayerUI();	//플레이어 UI 생성

	//Create spawner each stage
	void CreateMonsterSpawner();

	void ActivateStageMonsterSpawner(int stage_num);

	virtual bool ProcessInput(UINT id, WPARAM w_param, LPARAM l_param, float time) override;
	virtual const std::list<MeshComponent*>& GetShadowMeshList(int index) override;

	virtual void Update(float elapsed_time) override;

	virtual void RenderText(ID2D1DeviceContext2* d2d_device_context) override;

	virtual void AddObject(Object* object) override;
	virtual void DeleteObject(Object* object) override;
	virtual void DeleteDeadObjects() override;

	void UpdateObjectIsGround();
	void UpdateObjectHitWall();
	void UpdateObjectHitBullet();
	void UpdateObjectHitObject();
	void UpdateStageClear();

	void PrepareGroundChecking();	//맵 바닥체크를 위한 사전 작업

	void CheckObjectIsGround(Object* object);
	void CheckPlayerHitWall(Object* object, MovementComponent* movement);
	void CheckObjectHitObject(Object* object);
	void CheckObjectHitBullet(Object* object);
	void CheckObjectHitFlamethrow(Object* object);
	void CheckPlayerHitGun(Object* object);
	void CheckPlayerHitPyramid(Object* object);
	void CheckSpawnBoxHitPlayer();

	void SpawnMonsterDamagedParticle(const XMFLOAT3& position, const XMFLOAT4& color);

	//setter
	void set_stage_clear_num(int value);

	//getter
	std::list<MonsterComponent*> monster_list() const;
	int stage_clear_num();


	void add_stage_clear_num();
	void add_catch_monster_num();
	void add_remote_player(int id, const std::string& name, const XMFLOAT4X4& value);
	void add_monster(uint32_t id,
		const XMFLOAT4X4& matrix,
		int32_t max_hp,
		int32_t max_shield,
		int32_t attack_force,
		int32_t monster_type_int);
	void add_drop_gun(int id, uint8_t gun_type, uint8_t upgrade_level, uint8_t element_type, const XMFLOAT4X4& matrix);

	void change_gun(uint32_t gun_id, const std::string& gun_name, uint8_t upgrade_level, uint8_t element_type, uint32_t player_id);
	void OpenScrollChest(uint8_t scroll_type, uint8_t chest_num);
	void TakeScroll(uint8_t chest_num);

private:
	//static constexpr int kStageMaxCount{ 8 };	// 게임 스테이지 총 개수

	ID3D12Device* device_{ nullptr };	//Direct3D 12 디바이스

	//스테이지별 몬스터 스포너 리스트
	std::array<std::list<SpawnerComponent*>, kStageMaxCount> stage_monster_spawner_list_;

	// 몬스터 잡은 횟수
	int catch_monster_num_{ 0 };
	// 열쇠를 먹은 횟수
	int get_key_num_{ 0 };

	// F키 누름
	bool f_key_{};

	//현재 스테이지의 스포터를 활성화 했는가?
	bool is_activate_spawner_ = false;

	struct WallCheckObject
	{
		Object* object{ nullptr };
		MovementComponent* movement{ nullptr };

		WallCheckObject(Object* obj, MovementComponent* move)
			: object(obj), movement(move) {
		}
	};

	std::list<RazerComponent*> razer_list_;	//레이저 리스트

	std::list<WallCheckObject> wall_check_object_list_;	//벽 체크가 필요한 객체들의 리스트(플레이어, monster, NPC)

	std::array<std::list<GroundColliderComponent*>, 8> stage_ground_collider_list_;	//스테이지 바닥 콜라이더 리스트

	std::array<std::list<MeshComponent*>, 8> stage_mesh_list_;

	std::list<MonsterComponent*> monster_list_;	//몬스터 리스트

	std::unique_ptr<ParticleSystem> particle_system_{ nullptr };	//파티클 시스템

	std::vector<Object*> monster_hit_particles_;

	std::vector<Object*> dropped_guns_;

	std::vector<Object*> chests_;

	std::vector<Object*> sounds_;

	std::vector<BoxColliderComponent*> spawn_boxs_{}; // 스테이지 몬스터 생성 체크를 위한 박스들

	std::vector<CutSceneTrack> cut_scene_tracks_{};

	std::unique_ptr<TextRenderer> text_renderer_{ nullptr }; // 텍스트 렌더러
	std::unordered_map<std::string, std::unique_ptr<TextFormat>> text_formats_;
};

