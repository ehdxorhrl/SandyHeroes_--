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

	//��ȯ ��: ���� ��ǥ�迡�� ��ŷ�� ����
	//����: ��ũ�� x, y��ǥ�� �޾� ��ŷ ������ ������Ʈ�鰣 �浹�˻縦 ����
	XMVECTOR GetPickingPointAtWorld(float sx, float sy, Object* picked_object);

	virtual void AddObject(Object* object);

	virtual void Update(float elapsed_time);

	void DeleteObject(Object* object);

	void DeleteDeadObjects();

	void UpdateObjectWorldMatrix();

	Object* FindObject(const std::string& object_name);
	ModelInfo* FindModelInfo(const std::string& name);

	
	static Mesh* FindMesh(const std::string& mesh_name, const std::vector<std::unique_ptr<Mesh>>& meshes);
	static Texture* FindTexture(const std::string& texture_name, const std::vector<std::unique_ptr<Texture>>& textures);
	static Material* FindMaterial(const std::string& material_name, const std::vector<std::unique_ptr<Material>>& materials);

	virtual Object* CreatePlayerObject(long long session_id);
	
	//getter
	const std::vector<std::unique_ptr<Mesh>>& meshes() const;

protected:
	std::list<std::unique_ptr<Object>> object_list_;
	std::list<std::unique_ptr<Object>> dead_object_list_;
	std::vector<std::unique_ptr<Mesh>> meshes_;
	std::vector<std::unique_ptr<ModelInfo>> model_infos_;
	std::vector<std::unique_ptr<Material>> materials_;
	std::vector<std::unique_ptr<Texture>> textures_;

	std::vector<Sector> sectors_;

	GameFramework* game_framework_{ nullptr };

	InputControllerComponent* main_input_controller_{ nullptr };

	float total_time_{ 0 };
	
	bool is_play_cutscene_{ false };

protected:
	//�׸��ڸ� ���� �Ϻ� base scene ��� ������ scene���� �ű�
	bool is_prepare_ground_checking_ = false;
	//�� �ٴ�üũ�� ���� �޽� �ݶ��̴� ����Ʈ �迭
	std::array<std::list<MeshColliderComponent*>, 8> checking_maps_mesh_collider_list_;
	std::list<Object*> ground_check_object_list_;	//���� üũ�� �ʿ��� ��ü���� ����Ʈ(�÷��̾�, monster, NPC)

	// �÷��̾��� �������� ���൵
	int stage_clear_num_{ 0 };

};

