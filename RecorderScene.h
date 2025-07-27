#pragma once
#include "Scene.h"
#include "CutSceneTrack.h"

//ÄÆ½ÅÀ» ³ìÈ­ÇÏ´Â ¾À
class RecorderScene :
    public Scene
{
public:
	virtual void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* command_list,
		ID3D12RootSignature* root_signature, GameFramework* game_framework) override;
	virtual void BuildShader(ID3D12Device* device, ID3D12RootSignature* root_signature) override;
	virtual void BuildMesh(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;
	virtual void BuildMaterial(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;
	virtual void BuildObject(ID3D12Device* device, ID3D12GraphicsCommandList* command_list) override;

	virtual bool ProcessInput(UINT id, WPARAM w_param, LPARAM l_param, float time) override;

	virtual void Update(float elapsed_time) override;

private:
	std::unique_ptr<CutSceneRecorder> cut_scene_recorder_{ nullptr };

	std::vector<CutSceneTrack> cut_scene_tracks_{};
};

