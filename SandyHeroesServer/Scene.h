#pragma once
#include "Object.h"
#include "Material.h"
#include "Mesh.h"
#include "Sector.h"
#include "ModelInfo.h"

class InputControllerComponent;
class GameFramework;
class ColliderComponent;
class MeshColliderComponent;

class Scene
{
public:
	Scene() {}
	virtual ~Scene() {};

	virtual void Initialize(GameFramework* game_framework);
	virtual void BuildObject() = 0;
	virtual void BuildMesh() = 0;

	void BuildScene(const std::string& scene_name);

	virtual bool CheckObjectByObjectCollisions() { return false; };

	virtual bool ProcessInput(void* p) = 0;

	//반환 값: 월드 좌표계에서 피킹된 지점
	//설명: 스크린 x, y좌표를 받아 피킹 광선과 오브젝트들간 충돌검사를 시행
	XMVECTOR GetPickingPointAtWorld(float sx, float sy, Object* picked_object);

	virtual void AddObject(std::shared_ptr<Object> object);
	virtual void DeleteObject(std::shared_ptr<Object> object);

	virtual void Update(float elapsed_time);


	void UpdateObjectWorldMatrix();

	std::shared_ptr<Object> FindObject(const std::string& object_name);
	ModelInfo* FindModelInfo(const std::string& name);

	
	static Mesh* FindMesh(const std::string& mesh_name, const std::vector<std::unique_ptr<Mesh>>& meshes);
	static Texture* FindTexture(const std::string& texture_name, const std::vector<std::unique_ptr<Texture>>& textures);
	static Material* FindMaterial(const std::string& material_name, const std::vector<std::unique_ptr<Material>>& materials);

	virtual std::shared_ptr<Object> CreatePlayerObject(long long session_id);
	
	//getter
	const std::vector<std::unique_ptr<Mesh>>& meshes() const;

protected:	
	bool is_updating_objects_ = false;	//씬이 오브젝트들을 업데이트 중인지 여부.
	std::list<std::shared_ptr<Object>> add_object_list_;	//업데이트 중에 추가되는 오브젝트들을 임시로 저장하는 리스트. 업데이트가 끝나면 이 리스트의 오브젝트들이 씬의 오브젝트 리스트로 옮겨진다.
	std::list<std::shared_ptr<Object>> delete_object_list_;	//업데이트 중에 삭제되는 오브젝트들을 임시로 저장하는 리스트. 업데이트가 끝나면 이 리스트의 오브젝트들이 씬의 오브젝트 리스트에서 제거된다.

	std::list<std::shared_ptr<Object>> object_list_;
	std::vector<std::unique_ptr<Mesh>> meshes_;
	std::vector<std::unique_ptr<ModelInfo>> model_infos_;
	std::vector<std::unique_ptr<Material>> materials_;
	std::vector<std::unique_ptr<Texture>> textures_;

	std::vector<Sector> sectors_;

	GameFramework* game_framework_{ nullptr };

	std::shared_ptr<InputControllerComponent> main_input_controller_{ nullptr };

	float total_time_{ 0 };
	
	bool is_play_cutscene_{ false };

protected:
	//그림자를 위해 일부 base scene 멤버 변수들 scene으로 옮김
	bool is_prepare_ground_checking_ = false;
	//맵 바닥체크를 위한 메쉬 콜라이더 리스트 배열
	std::array<std::list<std::weak_ptr<MeshColliderComponent>>, 8> checking_maps_mesh_collider_list_;
	std::list<std::weak_ptr<Object>> ground_check_object_list_;	//지면 체크가 필요한 객체들의 리스트(플레이어, monster, NPC)

	// 플레이어의 스테이지 진행도
	int stage_clear_num_{ 0 };

};

