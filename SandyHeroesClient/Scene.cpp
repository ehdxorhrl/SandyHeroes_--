#include "stdafx.h"
#include "Scene.h"
#include "FrameResourceManager.h"
#include "DescriptorManager.h"
#include "CameraComponent.h"
#include "InputManager.h"
#include "InputControllerComponent.h"
#include "GameFramework.h"
#include "MeshComponent.h"
#include "SkinnedMeshComponent.h"
#include "MeshColliderComponent.h"
#include "ParticleComponent.h"
#include "SkinnedMesh.h"
#include "DDSTextureLoader.h"
#include "UIMesh.h"
#include "Sector.h"
#include "WallColliderComponent.h"


XMVECTOR Scene::GetPickingPointAtWorld(float sx, float sy, Object* picked_object)
{
	//picking ray 계산
	Object* camera_object = main_camera_->owner();

	XMVECTOR picking_point_w = XMLoadFloat3(&(camera_object->world_position_vector() + (camera_object->world_look_vector() * 100.f)));
	float picking_length_min = std::numeric_limits<float>::max();

	XMFLOAT4X4 proj = main_camera_->projection_matrix();

	float vx = (2.f * sx / kDefaultFrameBufferWidth - 1.f) / proj(0, 0);
	float vy = (-2.f * sy / kDefaultFrameBufferHeight + 1.f) / proj(1, 1);

	XMMATRIX view = XMLoadFloat4x4(&main_camera_->view_matrix());
	XMMATRIX inverse_view = XMMatrixInverse(&XMMatrixDeterminant(view), view);

	//월드좌표계에서 피킹광선
	XMVECTOR ray_origin{ XMVectorSet(0, 0, 0, 1.f) };
	XMVECTOR ray_direction{ XMVectorSet(vx, vy, 1.f, 0) };

	ray_origin = XMVector3TransformCoord(ray_origin, inverse_view);
	ray_direction = XMVector3Normalize(XMVector3Transform(ray_direction, inverse_view));

	for (const auto& mesh : meshes_)
	{
		if (mesh->name() == "Debug_Mesh")
			continue;
		const auto& mesh_component_list = mesh->mesh_component_list();
		for (const auto& mesh_component : mesh_component_list)
		{
			XMMATRIX world = XMLoadFloat4x4(&mesh_component->owner()->world_matrix());
			XMMATRIX inverse_world = XMMatrixInverse(&XMMatrixDeterminant(world), world);

			// 메쉬 로컬좌표의 피킹반직선
			XMVECTOR ray_origin_local = XMVector3TransformCoord(ray_origin, inverse_world);
			XMVECTOR ray_direction_local = XMVector3Normalize(XMVector3Transform(ray_direction, inverse_world));

			float t_min{ 0 }; // 반직선 교점 매개변수 최저값, 즉 가장 가까운 교점의 매개변수
			if (mesh->bounds().Intersects(ray_origin_local, ray_direction_local, t_min))
			{
				auto& positions = mesh->positions();
				auto& indices_array = mesh->indices_array();

				t_min = std::numeric_limits<float>::max();
				for (auto& indices : indices_array)
				{
					float t{ 0 };
					for (int i = 0; i < indices.size(); i += 3)
					{
						UINT i0 = indices[i + 0];
						UINT i1 = indices[i + 1];
						UINT i2 = indices[i + 2];

						XMVECTOR v0 = XMLoadFloat3(&positions[i0]);
						XMVECTOR v1 = XMLoadFloat3(&positions[i1]);
						XMVECTOR v2 = XMLoadFloat3(&positions[i2]);

						if (TriangleTests::Intersects(ray_origin_local, ray_direction_local, v0, v1, v2, t))
						{
							if (t < t_min) // 가장 반직선에 가까운 삼각형과의 교점 매개변수
							{
								t_min = t;
							}
						}
					}
				}	//forend indices_array
				if (t_min < std::numeric_limits<float>::max()) // t_min 유효한지 실제 메쉬와 부딪힌건지 체크
				{
					// 피킹지점을 월드 좌표계와 비교
					XMVECTOR picking_point = ray_origin_local + (ray_direction_local * t_min);
					picking_point = XMVector3TransformCoord(picking_point, world);
					float length = XMVectorGetX(XMVector3Length(picking_point - ray_origin));
					if (length < picking_length_min)
					{
						picking_length_min = length;
						picking_point_w = picking_point;
						picked_object = mesh_component->owner();
					}
				}
			}	//ifend mesh->bounds().Intersects
		}
	}

	return picking_point_w;
}

void Scene::Update(float elapsed_time)
{
	for (const std::unique_ptr<Object>& object : object_list_)
	{
		object->Update(elapsed_time);
	}

	UpdateSector();

	total_time_ += elapsed_time;
}

void Scene::DeleteDeadObjects()
{
	for (auto& sector : sectors_)
	{
		sector.DeleteDeadObject();
	}

	auto it = object_list_.begin();
	while (it != object_list_.end()) {
		if ((*it)->is_dead()) {
			auto current = it++;
			(*current)->Destroy();
			dead_object_list_.splice(dead_object_list_.end(), object_list_, current);
		}
		else {
			Object* dead_object = (*it)->PopDeadChild();
			while(dead_object)
			{
				dead_object_list_.emplace_back(std::unique_ptr<Object>(dead_object));
				dead_object = (*it)->PopDeadChild();
			}
			++it;
		}
	}

	dead_object_list_.remove_if([](const std::unique_ptr<Object>& object) {
			return (object->dead_frame_count() > FrameResourceManager::kFrameCount);
		});	

	for (const std::unique_ptr<Object>& object : dead_object_list_)
	{
		object->AddDeadFrameCount(1);
	}

}

void Scene::UpdateObjectWorldMatrix()
{
	for (const std::unique_ptr<Object>& object : object_list_)
	{
		object->UpdateWorldMatrix(nullptr);
	}
}

void Scene::UpdateSector()
{
	for (auto& sector : sectors_)
	{
		//섹터로 부터 벗어난 오브젝트를 해당 섹터에서 삭제
		sector.DeleteOutOfBoundsObjects();
	}

	for (const auto& object : object_list_)
	{
		if(!object->is_movable())
			continue;
		bool is_inserted = false;
		for (auto& sector : sectors_)
		{
			if (sector.CheckObjectInSectorObjectList(object.get()))
			{
				is_inserted = true;
				break;
			}
		}
		if (is_inserted)
			continue;
		for (auto& sector : sectors_)
		{
			if (sector.InsertObject(object.get()))
				break;
		}
	}
}

void Scene::RunViewFrustumCulling()
{
	int i = 0;
	int j = 0;
	BoundingFrustum world_frustum;
	auto view = XMLoadFloat4x4(&main_camera_->view_matrix());
	auto inv_view = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	main_camera_->view_frustum().Transform(world_frustum, inv_view);

	for (auto& sector : sectors_)
	{
		bool is_in_view_sector = sector.bounds().Intersects(world_frustum);
		for (const auto& object : sector.object_list())
		{
			object->set_is_in_view_sector(is_in_view_sector);
		}
	}
	//std::string str = "View Frustum Culling: " + std::to_string(i) + " objects checked.\n" + std::to_string(j) + " Sectors checked.\n";
	//std::wstring wstr(str.begin(), str.end());
	//OutputDebugString(wstr.c_str());
}

Object* Scene::FindObject(const std::string& object_name)
{
	auto it = std::find_if(object_list_.begin(), object_list_.end(), [&object_name](const std::unique_ptr<Object>& object) {
		return object.get()->name() == object_name;
		});

	if (it != object_list_.end())
	{
		return (*it).get();
	}

	return nullptr;
}

Object* Scene::FindObject(const long long id)
{
	auto it = std::find_if(object_list_.begin(), object_list_.end(), [&id](const std::unique_ptr<Object>& object) {
		return object.get()->id() == id;
		});

	if (it != object_list_.end())
	{
		return (*it).get();
	}

	return nullptr;
}

ModelInfo* Scene::FindModelInfo(const std::string& name)
{
	auto it = std::find_if(model_infos_.begin(), model_infos_.end(), [&name](const std::unique_ptr<ModelInfo>& object) {
		return object.get()->model_name() == name;
		});

	if (it != model_infos_.end())
	{
		return (*it).get();
	}
	return nullptr;
}


Mesh* Scene::FindMesh(const std::string& mesh_name, const std::vector<std::unique_ptr<Mesh>>& meshes)
{
	auto it = std::find_if(meshes.begin(), meshes.end(), [&mesh_name](const std::unique_ptr<Mesh>& mesh) {
		return mesh.get()->name() == mesh_name;
		});

	if (it != meshes.end())
	{
		return (*it).get();
	}

	return nullptr;
}

Material* Scene::FindMaterial(const std::string& material_name, const std::vector<std::unique_ptr<Material>>& materials)
{
	auto it = std::find_if(materials.begin(), materials.end(), [&material_name](const std::unique_ptr<Material>& mat) {
		return mat.get()->name() == material_name;
		});

	if (it != materials.end())
	{
		return (*it).get();
	}

	return nullptr;
}

Texture* Scene::FindTexture(const std::string& texture_name, const std::vector<std::unique_ptr<Texture>>& textures)
{
	auto it = std::find_if(textures.begin(), textures.end(), [&texture_name](const std::unique_ptr<Texture>& tex) {
		return tex.get()->name == texture_name;
		});

	if (it != textures.end())
	{
		return (*it).get();
	}
	return nullptr;
}

const std::vector<std::unique_ptr<Mesh>>& Scene::meshes() const
{
	return meshes_;
}

CameraComponent* Scene::main_camera() const
{
	return main_camera_;
}

XMFLOAT2 Scene::screen_size() const
{
	return game_framework_->client_size();
}

bool Scene::is_play_cutscene() const
{
	return is_play_cutscene_;
}

Object* Scene::player() const
{
	return player_;
}

void Scene::set_main_camera(CameraComponent* value)
{
	main_camera_ = value;
}

void Scene::set_is_play_cutscene(bool value)
{
	is_play_cutscene_ = value;
}

void Scene::AddObject(Object* object)
{
	object_list_.emplace_back();
	object_list_.back().reset(object);
}

void Scene::DeleteObject(Object* object)
{
	for (auto& sector : sectors_)
	{
		sector.DeleteObject(object);
	}

	object_list_.remove_if([&object](const std::unique_ptr<Object>& obj) {
		return obj.get() == object;
		});
}

void Scene::ReleaseMeshUploadBuffer()
{
	for (std::unique_ptr<Mesh>& mesh : meshes_)
	{
		mesh->ReleaseUploadBuffer();
	}
}

void Scene::UpdateRenderPassConstantBuffer(ID3D12GraphicsCommandList* command_list)
{
	main_camera_->UpdateCameraInfo();

	CBPass cb_pass{};
	cb_pass.view_matrix = xmath_util_float4x4::TransPose(main_camera_->view_matrix());
	cb_pass.proj_matrix = xmath_util_float4x4::TransPose(main_camera_->projection_matrix());
	cb_pass.camera_position = main_camera_->world_position();
	cb_pass.camera_up_axis = main_camera_->up_vector();

	cb_pass.ambient_light = XMFLOAT4{ 0.01,0.01,0.01, 1 };
	cb_pass.lights[0].strength = XMFLOAT3{ 0.7, 0.7, 0.7 };
	cb_pass.lights[0].direction = XMFLOAT3{ 0.577350020,  -0.577350020, 0.577350020 };
	cb_pass.lights[0].enable = true;
	cb_pass.lights[0].type = 0;

	cb_pass.total_time = total_time_;

	cb_pass.screen_size = game_framework_->client_size();

	//cb_pass.lights[1].strength = XMFLOAT3{ 1, 0, 0 };
	//cb_pass.lights[1].falloff_start = 0.1;
	//cb_pass.lights[1].direction = xmath_util_float3::Normalize(main_camera_->owner()->world_look_vector());
	//cb_pass.lights[1].falloff_end = 100.f;
	//cb_pass.lights[1].position = main_camera_->owner()->world_position_vector();
	//cb_pass.lights[1].spot_power = 14;
	//cb_pass.lights[1].enable = true;
	//cb_pass.lights[1].type = 2;

	for (int i = 1; i < 16; ++i)
		cb_pass.lights[i].enable = false;

	FrameResourceManager* frame_resource_manager = game_framework_->frame_resource_manager();
	frame_resource_manager->curr_frame_resource()->cb_pass.get()->CopyData(0, cb_pass);

	//25.02.23 수정
	//기존 루트 디스크립터 테이블에서 루트 CBV로 변경
	D3D12_GPU_VIRTUAL_ADDRESS cb_pass_address =
		frame_resource_manager->curr_frame_resource()->cb_pass.get()->Resource()->GetGPUVirtualAddress();

	command_list->SetGraphicsRootConstantBufferView((int)RootParameterIndex::kRenderPass, cb_pass_address);
}

void Scene::UpdateRenderPassShadowBuffer(ID3D12GraphicsCommandList* command_list)
{
	constexpr float radius = 150.0f;

	CBShadow shadow_pass = {};
	shadow_pass.light_dir = { 0.577350020,  -0.577350020, 0.577350020 };

	//// Only the first "main" light casts a shadow.
	//XMVECTOR lightDir = XMLoadFloat3(&shadow_pass.light_dir);
	//XMVECTOR targetPos = XMVectorSet(0, 0, 0, 1);
	//XMVECTOR lightPos = (-2.0f * radius * lightDir);
	//XMVECTOR lightUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	//XMMATRIX lightView = XMMatrixLookAtLH(lightPos, targetPos, lightUp);

	//XMStoreFloat3(&shadow_pass.light_pos_w, lightPos);


	auto camera_object = main_camera_->owner();
	XMFLOAT3 player_pos = camera_object->world_position_vector();
	XMVECTOR targetPos = XMLoadFloat3(&player_pos); // 플레이어를 타겟으로
	XMVECTOR lightDir = XMLoadFloat3(&shadow_pass.light_dir);
	XMVECTOR lightPos = targetPos - 2.0f * radius * lightDir;
	XMVECTOR lightUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX lightView = XMMatrixLookAtLH(lightPos, targetPos, lightUp);
	XMStoreFloat3(&shadow_pass.light_pos_w, lightPos);


	// Transform bounding sphere to light space.
	XMFLOAT3 sphereCenterLS;
	XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, lightView));

	// Ortho frustum in light space encloses scene.
	float l = sphereCenterLS.x - radius;
	float b = sphereCenterLS.y - radius;
	float n = sphereCenterLS.z - radius;
	float r = sphereCenterLS.x + radius;
	float t = sphereCenterLS.y + radius;
	float f = sphereCenterLS.z + radius;

	shadow_pass.near_z = n;
	shadow_pass.far_z = f;
	XMMATRIX lightProj = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX S = lightView * lightProj * T;
	XMStoreFloat4x4(&shadow_pass.light_view, XMMatrixTranspose(lightView));
	XMStoreFloat4x4(&shadow_pass.light_proj, XMMatrixTranspose(lightProj));
	XMStoreFloat4x4(&shadow_pass.shadow_transform, XMMatrixTranspose(S));

	FrameResourceManager* frame_resource_manager = game_framework_->frame_resource_manager();
	frame_resource_manager->curr_frame_resource()->cb_shadow.get()->CopyData(0, shadow_pass);

	D3D12_GPU_VIRTUAL_ADDRESS cb_shadow_address =
		frame_resource_manager->curr_frame_resource()->cb_shadow.get()->Resource()->GetGPUVirtualAddress();

	command_list->SetGraphicsRootConstantBufferView((int)RootParameterIndex::kShadowPass, cb_shadow_address);
}

void Scene::UpdateObjectConstantBuffer(FrameResource* curr_frame_resource)
{
	int cb_object_index = 0;
	int cb_skinned_mesh_index = 0;
	int cb_ui_mesh_index = 0;
	for (const auto& mesh : meshes_)
	{
		auto skinned_mesh = dynamic_cast<SkinnedMesh*>(mesh.get());
		auto ui_mesh = dynamic_cast<UIMesh*>(mesh.get());
		if (skinned_mesh)
		{
			skinned_mesh->UpdateConstantBuffer(curr_frame_resource, cb_skinned_mesh_index);
		}
		else if (ui_mesh)
		{
			ui_mesh->UpdateConstantBuffer(curr_frame_resource, cb_ui_mesh_index);
		}
		else
		{
			mesh->UpdateConstantBuffer(curr_frame_resource, cb_object_index);
		}
	}
}

void Scene::Render(ID3D12GraphicsCommandList* command_list)
{
	FrameResourceManager* frame_resource_manager = game_framework_->frame_resource_manager();
	auto curr_frame_resource = frame_resource_manager->curr_frame_resource();

	//RunViewFrustumCulling();

	//Default-Render-Pass
	//UpdateRenderPassConstantBuffer(command_list);
	//UpdateObjectConstantBuffer(curr_frame_resource);

	for (const auto& [type, shader] : shaders_)
	{
		if (shader->shader_type() == ShaderType::kDebug && !is_render_debug_mesh_)
		{
			continue;
		}
		shader->Render(command_list, curr_frame_resource, game_framework_->descriptor_manager(), main_camera_);
	}
}

void Scene::ShadowRender(ID3D12GraphicsCommandList* command_list)
{
	FrameResourceManager* frame_resource_manager = game_framework_->frame_resource_manager();
	auto curr_frame_resource = frame_resource_manager->curr_frame_resource();

	//Shadow-Render-Pass
	UpdateRenderPassConstantBuffer(command_list);
	UpdateRenderPassShadowBuffer(command_list);
	UpdateObjectConstantBuffer(curr_frame_resource);

	{
		auto& skinnedShadow = shaders_[(int)ShaderType::kSkinnedShadow];
		auto& skinnedShader = shaders_[(int)ShaderType::kStandardSkinnedMesh];
		skinnedShadow->Render(command_list, curr_frame_resource, game_framework_->descriptor_manager(), main_camera_);
		skinnedShader->Render(command_list, curr_frame_resource, game_framework_->descriptor_manager(), main_camera_, true);
	}

	//{
	//	auto& shadow_shader = shaders_[(int)ShaderType::kShadow];
	//	auto& mesh_shader = shaders_[(int)ShaderType::kStandardMesh];
	//	shadow_shader->Render(command_list, curr_frame_resource, game_framework_->descriptor_manager(), main_camera_);
	//	mesh_shader->Render(command_list, curr_frame_resource, game_framework_->descriptor_manager(), main_camera_, true);
	//}

	auto& shadow_shader = shaders_[(int)ShaderType::kShadow];
	shadow_shader->Render(command_list, curr_frame_resource, game_framework_->descriptor_manager(), main_camera_);
	for (int i = -1; i <= 1; ++i)
	{
		int idx = std::clamp(stage_clear_num_ + i, 0, 7);
		const auto& mesh_list_ = GetShadowMeshList(idx);
		for (auto& mesh_component : mesh_list_)
		{
			mesh_component->UpdateConstantBufferForShadow(curr_frame_resource, -1);

			auto gpu_address = curr_frame_resource->cb_object->Resource()->GetGPUVirtualAddress();
			const auto cb_size = d3d_util::CalculateConstantBufferSize((sizeof(CBObject)));
			gpu_address += cb_size * mesh_component->constant_buffer_index();

			command_list->SetGraphicsRootConstantBufferView((int)RootParameterIndex::kWorldMatrix, gpu_address);

			mesh_component->GetMesh()->Render(command_list, 0);
		}
	}

}

void Scene::ParticleRender(ID3D12GraphicsCommandList* command_list)
{
	auto& particleShader = shaders_[(int)ShaderType::kParticle];
	FrameResourceManager* frame_resource_manager = game_framework_->frame_resource_manager();
	auto curr_frame_resource = frame_resource_manager->curr_frame_resource();

	
	particleShader->Render(command_list, curr_frame_resource, game_framework_->descriptor_manager(), main_camera_);

	particle_renderer_->ParticleRender(command_list, curr_frame_resource, game_framework_->descriptor_manager(), main_camera_);

}

void Scene::RenderText(ID2D1DeviceContext2* d2d_device_context)
{

}

Scene::~Scene()
{
	object_list_.clear();
	model_infos_.clear();
	materials_.clear();
	meshes_.clear();

}

void Scene::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* command_list,
	ID3D12RootSignature* root_signature, GameFramework* game_framework, ID2D1DeviceContext* device_context, IDWriteFactory* dwrite_factory)
{
	game_framework_ = game_framework;

	particle_renderer_ = std::make_unique<ParticleRenderer>();
	ParticleComponent::kParticleRenderer = particle_renderer_.get();

	BuildTextBrushAndFormat(device_context, dwrite_factory);
	BuildShader(device, root_signature);
	BuildMesh(device, command_list);
	BuildMaterial(device, command_list);
	BuildObject(device, command_list);
	InitializeSectorObjectlist();
	BuildFrameResources(device);
	BuildDescriptorHeap(device);
	BuildConstantBufferViews(device);
	BuildShaderResourceViews(device);
}

void Scene::BuildMaterial(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)
{
	int frame_resource_index = 0;
	for (std::unique_ptr<Material>& material : materials_)
	{
		shaders_[material->shader_type()]->AddMaterial(material.get());
		material->set_frame_resource_index(frame_resource_index);
		++frame_resource_index;
	}

	for (const auto& const texture : textures_)
	{
		std::string file_name = "./Resource/Model/Texture/DDS/" + texture->name + ".dds";
		std::wstring file_name_w;
		file_name_w.assign(file_name.begin(), file_name.end());
		DirectX::CreateDDSTextureFromFile12(device, command_list,
			file_name_w.c_str(),
			texture->resource, texture->upload_heap);
	}
}

void Scene::BuildFrameResources(ID3D12Device* device)
{
	game_framework_->frame_resource_manager()->
		ResetFrameResources(device, 1, cb_object_capacity_,
			cb_skinned_mesh_object_capacity_, materials_.size(), cb_ui_mesh_capacity_);
}

void Scene::BuildDescriptorHeap(ID3D12Device* device)
{
	game_framework_->descriptor_manager()->
		ResetDescriptorHeap(device,
			textures_.size() + 1);	// + 1 is ShadowMap
}

void Scene::BuildShaderResourceViews(ID3D12Device* device)
{
	int i = game_framework_->descriptor_manager()->srv_offset();
	auto descriptor_manager = game_framework_->descriptor_manager();

	D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc{};
	srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srv_desc.Texture2D.MostDetailedMip = 0;
	srv_desc.Texture2D.ResourceMinLODClamp = 0.f;

	D3D12_CPU_DESCRIPTOR_HANDLE descriptor;

	for (const auto& const texture : textures_)
	{
		descriptor = descriptor_manager->GetCpuHandle(i);
		srv_desc.Format = texture->resource->GetDesc().Format;
		srv_desc.Texture2D.MipLevels = texture->resource->GetDesc().MipLevels;
		if (texture->type == TextureType::kCubeMap)
		{
			srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		}
		else
		{
			srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		}
		device->CreateShaderResourceView(texture->resource.Get(), &srv_desc, descriptor);
		texture->heap_index = i;
		++i;
	}
}

void Scene::BuildTextBrushAndFormat(ID2D1DeviceContext* device_context, IDWriteFactory* dwrite_factory)
{
	device_context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Blue), &d2d_text_brush_);
	dwrite_factory->CreateTextFormat(
		L"맑은 고딕",
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		20,
		L"en-us",
		&d2d_text_format_
	);
	d2d_text_format_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	d2d_text_format_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
}

using namespace file_load_util;
void Scene::BuildScene()
{
	std::ifstream scene_file{ "./Resource/Model/World/Scene.bin", std::ios::binary };

	int root_object_count = ReadFromFile<int>(scene_file);

	std::string load_token;

	for (int i = 0; i < root_object_count; ++i)
	{
		ReadStringFromFile(scene_file, load_token);
		if (load_token[0] == '@')
		{
			load_token.erase(0, 1);
			object_list_.emplace_back();
			object_list_.back().reset(FindModelInfo(load_token)->GetInstance());

			ReadStringFromFile(scene_file, load_token);
			XMFLOAT4X4 transfrom = ReadFromFile<XMFLOAT4X4>(scene_file);
			object_list_.back()->set_transform_matrix(transfrom);
		}
		else
		{
			std::string object_name = load_token;

			ReadStringFromFile(scene_file, load_token); // <Transfrom>
			XMFLOAT4X4 transfrom = ReadFromFile<XMFLOAT4X4>(scene_file);

			model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/World/" + object_name + ".bin", meshes_, materials_, textures_));

			object_list_.emplace_back();
			object_list_.back().reset(model_infos_.back()->GetInstance());

			object_list_.back()->set_transform_matrix(transfrom);

		}
	}
}

void Scene::InitializeSectorObjectlist()
{
	for (const auto& object : object_list_)
	{
		bool is_inserted = false;
		for (auto& sector : sectors_)
		{
			if (sector.CheckObjectInSectorObjectList(object.get()))
			{
				is_inserted = true;
				break;
			}
		}
		if (is_inserted)
			continue;
		for (auto& sector : sectors_)
		{
			if (sector.InsertObject(object.get()))
				break;
		}
	}
}

const std::list<MeshComponent*>& Scene::GetShadowMeshList(int index)
{
	return std::list<MeshComponent*>();
}