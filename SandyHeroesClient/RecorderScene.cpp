#include "stdafx.h"
#include "RecorderScene.h"
#include "StandardMeshShader.h"
#include "StandardSkinnedMeshShader.h"
#include "SkyboxShader.h"
#include "CubeMesh.h"
#include "SkyboxMesh.h"
#include "ModelInfo.h"
#include "GameFramework.h"
#include "CameraComponent.h"
#include "TestControllerComponent.h"
#include "MovementComponent.h"
#include "MeshComponent.h"

void RecorderScene::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* command_list, 
	ID3D12RootSignature* root_signature, GameFramework* game_framework,
	ID2D1DeviceContext* device_context, IDWriteFactory* dwrite_factory)
{
	cut_scene_recorder_ = std::make_unique<CutSceneRecorder>("Test1");

	constexpr int kCutSceneTrackCount = 1;
	cut_scene_tracks_.reserve(kCutSceneTrackCount);
	cut_scene_tracks_.emplace_back("CutScene");

	Scene::Initialize(device, command_list, root_signature, game_framework, device_context, dwrite_factory);

}

void RecorderScene::BuildShader(ID3D12Device* device, ID3D12RootSignature* root_signature)
{
	constexpr int shader_count = 3;
	shaders_.reserve(shader_count);
	shaders_[(int)ShaderType::kStandardMesh] = std::make_unique<StandardMeshShader>();
	shaders_[(int)ShaderType::kStandardSkinnedMesh] = std::make_unique<StandardSkinnedMeshShader>();
	shaders_[(int)ShaderType::kSkybox] = std::make_unique<SkyboxShader>();

	for (int i = 0; i < shaders_.size(); ++i)
	{
		shaders_[i]->CreateShader(device, root_signature);
	}
}

using namespace file_load_util;
void RecorderScene::BuildMesh(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)
{
	constexpr UINT kMeshCount{ 60 };
	meshes_.reserve(kMeshCount);
	meshes_.push_back(std::make_unique<CubeMesh>());
	meshes_.back().get()->set_name("green_cube");

	//skybox
	meshes_.push_back(std::make_unique<SkyboxMesh>(meshes_[0].get()));

	//debug mesh
	Mesh* debug_mesh = new CubeMesh();
	debug_mesh->ClearNormals();
	debug_mesh->ClearNormals();
	debug_mesh->ClearTangents();
	debug_mesh->set_name("Debug_Mesh");
	meshes_.emplace_back();
	meshes_.back().reset(debug_mesh);

	model_infos_.reserve(40);
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Dog00.bin", meshes_, materials_, textures_));
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Gun/classic.bin", meshes_, materials_, textures_));
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Gun/SM_Bullet_01.bin", meshes_, materials_, textures_));
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Monster/Hit_Dragon.bin", meshes_, materials_, textures_));
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Monster/Shot_Dragon.bin", meshes_, materials_, textures_));

	BuildScene();

	for (const std::unique_ptr<Mesh>& mesh : meshes_)
	{
		mesh->CreateShaderVariables(device, command_list);
	}
}

void RecorderScene::BuildMaterial(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)
{
	Material* material = new Material{ "green", (int)ShaderType::kStandardMesh, {0, 1, 0, 1} };
	materials_.emplace_back();
	materials_.back().reset(material);

	material = new Material{ "Skybox_Cube2", (int)ShaderType::kSkybox };
	textures_.push_back(std::make_unique<Texture>());
	textures_.back()->name = "Skybox_Cube2";
	textures_.back()->type = TextureType::kCubeMap;
	material->AddTexture(textures_.back().get());
	materials_.emplace_back();
	materials_.back().reset(material);

	Scene::BuildMaterial(device, command_list);
}

void RecorderScene::BuildObject(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)
{
	cb_object_capacity_ = 5000;
	cb_skinned_mesh_object_capacity_ = 5000;

	//Create Skybox
	Object* skybox = new Object();
	skybox->AddComponent(new MeshComponent(skybox, Scene::FindMesh("Skybox", meshes_)));
	AddObject(skybox);

	//카메라 배치
	Object* camera = new Object{};
	camera->set_name("Camera");
	camera->set_position_vector(XMFLOAT3{ 0.f, 5.5f, -10.f });

	CameraComponent* camera_component = new CameraComponent{ camera, 0.1, 1000, 
		(float)kDefaultFrameBufferWidth / (float)kDefaultFrameBufferHeight, 58.f };
	camera->AddComponent(camera_component);
	main_camera_ = camera_component;

	TestControllerComponent* controller = new TestControllerComponent(camera);
	camera->AddComponent(controller);
	controller->set_client_wnd(game_framework_->main_wnd());
	main_input_controller_ = controller;

	MovementComponent* movement = new MovementComponent(camera);
	movement->DisableGarvity();
	camera->AddComponent(movement);

	AddObject(camera);

	cut_scene_recorder_->set_camera_object(camera);
	
	//컷씬 적용 카메라
	camera = new Object{};
	camera->set_name("CutSceneCamera");

	camera_component = new CameraComponent{ camera, 0.1, 1000,
	(float)kDefaultFrameBufferWidth / (float)kDefaultFrameBufferHeight, 58.f };
	camera->AddComponent(camera_component);

	cut_scene_tracks_.back().set_camera(camera);

	AddObject(camera);

}

bool RecorderScene::ProcessInput(UINT id, WPARAM w_param, LPARAM l_param, float time)
{
	if (main_input_controller_)
	{
		if (main_input_controller_->ProcessInput(id, w_param, l_param, time))
			return true;
	}
	switch (id)
	{
	case WM_KEYDOWN:
		switch (w_param)
		{
		case 'R':
			cut_scene_recorder_->Start();
			break;
		case VK_LMENU:
			cut_scene_recorder_->Pause();
			break;
		case 'L':
			cut_scene_recorder_->Stop();
			break;
		case 'P':
			cut_scene_tracks_[0].Play(this);
			break;
		default:
			return false;
			break;
		}
		break;
	default:
		return false;
		break;
	}

	return true;
}

void RecorderScene::Update(float elapsed_time)
{
	for (auto& cut_scene_track : cut_scene_tracks_)
	{
		cut_scene_track.Run(elapsed_time);
	}


	Scene::Update(elapsed_time);

	if (cut_scene_recorder_)
	{
		cut_scene_recorder_->CaptureCameraTransform(elapsed_time);
	}

}



