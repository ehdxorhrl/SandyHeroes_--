#pragma once
#include "Object.h"
#include "Shader.h"
#include "Material.h"
#include "Mesh.h"
#include "ModelInfo.h"

class FrameResourceManager;
class DescriptorManager;
class CameraComponent;
class InputManager;
class InputControllerComponent;
class GameFramework;
class ColliderComponent;
class MeshColliderComponent;

class Scene
{
public:
	Scene() {}
	virtual ~Scene() {};

	virtual void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* command_list, 
		ID3D12RootSignature* root_signature, GameFramework* game_framework);
	virtual void BuildShader(ID3D12Device* device, ID3D12RootSignature* root_signature) = 0;
	virtual void BuildMesh(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) = 0;
	virtual void BuildMaterial(ID3D12Device* device, ID3D12GraphicsCommandList* command_list);
	virtual void BuildObject(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) = 0;
	virtual void BuildFrameResources(ID3D12Device* device);
	virtual void BuildDescriptorHeap(ID3D12Device* device);
	virtual void BuildConstantBufferViews(ID3D12Device* device) {};
	virtual void BuildShaderResourceViews(ID3D12Device* device);
	
	void BuildScene();

	virtual bool CheckObjectByObjectCollisions() { return false; };

	void ReleaseMeshUploadBuffer();

	virtual void UpdateRenderPassConstantBuffer(ID3D12GraphicsCommandList* command_list);
	virtual void UpdateRenderPassShadowBuffer(ID3D12GraphicsCommandList* command_list);

	virtual void Render(ID3D12GraphicsCommandList* command_list);
	virtual void ShadowRender(ID3D12GraphicsCommandList* command_list);

	virtual bool ProcessInput(UINT id, WPARAM w_param, LPARAM l_param, float time) = 0;

	//반환 값: 월드 좌표계에서 피킹된 지점
	//설명: 스크린 x, y좌표를 받아 피킹 광선과 오브젝트들간 충돌검사를 시행
	XMVECTOR GetPickingPointAtWorld(float sx, float sy, Object* picked_object);

	virtual void Update(float elapsed_time);

	void UpdateObjectWorldMatrix();

	Object* FindObject(const std::string& object_name);
	Object* FindObject(const long long id);
	ModelInfo* FindModelInfo(const std::string& name);

	static Mesh* FindMesh(const std::string& mesh_name, const std::vector<std::unique_ptr<Mesh>>& meshes);
	static Material* FindMaterial(const std::string& material_name, const std::vector<std::unique_ptr<Material>>& materials);
	static Texture* FindTexture(const std::string& texture_name, const std::vector<std::unique_ptr<Texture>>& textures);

	//getter
	const std::vector<std::unique_ptr<Mesh>>& meshes() const;
	CameraComponent* main_camera() const;
	XMFLOAT2 screen_size() const;

	//setter
	void set_main_camera(CameraComponent* value);

protected:
	std::list<std::unique_ptr<Object>> object_list_;
	std::unordered_map<int, std::unique_ptr<Shader>> shaders_;
	std::vector<std::unique_ptr<Mesh>> meshes_;
	std::vector<std::unique_ptr<ModelInfo>> model_infos_;
	std::vector<std::unique_ptr<Material>> materials_;
	std::vector<std::unique_ptr<Texture>> textures_;

	GameFramework* game_framework_{ nullptr };

	// 일반 메쉬를 사용하는 오브젝트의 최대 용량
	// 이 용량만큼 상수버퍼가 gpu에 만들어진다.
	int cb_object_capacity_{ 0 };
	int cb_skinned_mesh_object_capacity_{ 0 };
	int cb_ui_mesh_capacity_{ 0 };

	CameraComponent* main_camera_{ nullptr };
	InputControllerComponent* main_input_controller_{ nullptr };

	bool is_render_debug_mesh_ = false;	//디버그용 와이어프레임 obb를 렌더하는지 여부

	float total_time_{ 0 };

protected:
	//그림자를 위해 일부 base scene 멤버 변수들 scene으로 옮김
	bool is_prepare_ground_checking_ = false;
	//맵 바닥체크를 위한 메쉬 콜라이더 리스트 배열
	std::array<std::list<MeshColliderComponent*>, 8> checking_maps_mesh_collider_list_;

	// 플레이어의 스테이지 진행도
	int stage_clear_num_{ 0 };

	//TODO: Player 객체 구현
	Object* player_ = nullptr;
};

