#pragma once
#include "Scene.h"
#include "CutSceneRecorder.h"

class RecordingScene :
    public Scene
{
public:
	RecordingScene() {}
	~RecordingScene() {}

	void BuildShader(ID3D12Device* device, ID3D12RootSignature* root_signature) override;
	void BuildMesh(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;
	void BuildObject(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;
	void Render(ID3D12GraphicsCommandList* command_list) override;
	bool ProcessInput(UINT id, WPARAM w_param, LPARAM l_param, float time) override;
	void Update(float elapsed_time) override;


};

