#pragma once
#include "Scene.h"

// 프레임워크 제작용 테스트 씬
// 씬의 가상함수들을 실제로 작성하고 실행해본다.
class TestScene :
    public Scene
{
public:
	TestScene() {}

	virtual void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* command_list,
		ID3D12RootSignature* root_signature, GameFramework* game_framework, 
		ID2D1DeviceContext* device_context, IDWriteFactory* dwrite_factory) override;
	virtual void BuildShader(ID3D12Device* device, ID3D12RootSignature* root_signature) override;
	virtual void BuildMesh(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;
	virtual void BuildObject(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;
	virtual void BuildConstantBufferViews(ID3D12Device* device) override;
	void BuildScene();

	virtual bool CheckObjectByObjectCollisions();

	virtual bool ProcessInput(UINT id, WPARAM w_param, LPARAM l_param, float time) override;


private:
	Object* player_ = nullptr;
};

