#pragma once
#include "Scene.h"
#include "ColliderComponent.h"

class MeshColliderComponent;

// 프레임워크 제작용 테스트 씬
// 씬의 가상함수들을 실제로 작성하고 실행해본다.
class AITestScene :
	public Scene
{
public:
	AITestScene() {}

	virtual void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* command_list,
		ID3D12RootSignature* root_signature, GameFramework* game_framework) override;
	virtual void BuildShader(ID3D12Device* device, ID3D12RootSignature* root_signature) override;
	virtual void BuildMesh(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;
	virtual void BuildObject(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;
	virtual void BuildConstantBufferViews(ID3D12Device* device) override;
	void BuildScene();

	virtual bool CheckObjectByObjectCollisions();

	virtual void Render(ID3D12GraphicsCommandList* command_list) override;

	virtual bool ProcessInput(UINT id, WPARAM w_param, LPARAM l_param, float time) override;

	void AddRemotePlayer(int id, const std::string& name, const XMFLOAT3& pos);

	Object* player() const { return player_; }

private:
	static constexpr int kStageMaxCount{ 8 };	// 게임 스테이지 총 개수
	bool is_prepare_ground_checking_ = false;
	std::array<std::list<MeshColliderComponent*>, kStageMaxCount> checking_maps_mesh_collider_list_;	//맵 바닥체크를 위한 메쉬 콜라이더 리스트 배열
	int stage_clear_num_{ 0 };	// 플레이어의 스테이지 진행도

	bool is_render_debug_mesh_ = false;	//디버그용 와이어프레임 obb를 렌더하는지 여부
};

