#pragma once
#include "Scene.h"
#include "ColliderComponent.h"

class MeshColliderComponent;

// �����ӿ�ũ ���ۿ� �׽�Ʈ ��
// ���� �����Լ����� ������ �ۼ��ϰ� �����غ���.
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
	static constexpr int kStageMaxCount{ 8 };	// ���� �������� �� ����
	bool is_prepare_ground_checking_ = false;
	std::array<std::list<MeshColliderComponent*>, kStageMaxCount> checking_maps_mesh_collider_list_;	//�� �ٴ�üũ�� ���� �޽� �ݶ��̴� ����Ʈ �迭
	int stage_clear_num_{ 0 };	// �÷��̾��� �������� ���൵

	bool is_render_debug_mesh_ = false;	//����׿� ���̾������� obb�� �����ϴ��� ����
};

