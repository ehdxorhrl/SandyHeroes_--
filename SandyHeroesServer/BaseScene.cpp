#include "stdafx.h"
#include "BaseScene.h"
#include "CubeMesh.h"
#include "GameFramework.h"
#include "MeshComponent.h"
#include "MovementComponent.h"
#include "BoxColliderComponent.h"
#include "AnimatorComponent.h"
#include "DebugMeshComponent.h"
#include "SkinnedMeshComponent.h"
#include "PlayerAnimationState.h"
#include "GunComponent.h"
#include "Object.h"
#include "User.h"
#include "ChestComponent.h"
#include "GroundColliderComponent.h"
#include "WallColliderComponent.h"
#include "SessionManager.h"
#include "MonsterComponent.h"
#include "SpawnerComponent.h"
#include "PlayerComponent.h"
#include "RazerMesh.h"
#include "AStar.h"
#include "BillboardMeshComponent.h"

using namespace file_load_util;
void BaseScene::BuildMesh()
{
	constexpr UINT kMeshCount{ 60 };
	meshes_.reserve(kMeshCount);
	meshes_.push_back(std::make_unique<CubeMesh>());
	meshes_.back().get()->set_name("green_cube");

	//CrossHair
	constexpr float cross_hair_size = 64.f;
	float ui_width = cross_hair_size;
	float ui_height = cross_hair_size;
	float ui_x = 0;
	float ui_y = 0;
	meshes_.push_back(std::make_unique<CubeMesh>());
	meshes_.back().get()->set_name("CrossHair");

	//Hp Bar
	constexpr float hp_bar_width = 100.f;
	constexpr float hp_bar_height = 15.f;
	ui_width = hp_bar_width;
	ui_height = hp_bar_height;
	meshes_.push_back(std::make_unique<CubeMesh>());
	meshes_.back().get()->set_name("ProgressBarBackground");
	ui_width = hp_bar_width - 5;
	ui_height = hp_bar_height - 5;
	meshes_.push_back(std::make_unique<CubeMesh>());
	meshes_.back().get()->set_name("ProgressBar");

	//skybox
	meshes_.push_back(std::make_unique<CubeMesh>());

	// 기본 디버그 메쉬 생성
	Mesh* debug_mesh = new CubeMesh();
	debug_mesh->ClearNormals();
	debug_mesh->ClearNormals();
	debug_mesh->ClearTangents();
	debug_mesh->set_name("Debug_Mesh");
	meshes_.emplace_back();
	meshes_.back().reset(debug_mesh);

	//RazerMesh
	meshes_.push_back(std::make_unique<RazerMesh>(0.08f, 50.f));

	// 외부 모델 로딩
	constexpr UINT kModelInfoCount{ 40 };
	model_infos_.reserve(kModelInfoCount);
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Dog00.bin", meshes_, materials_, textures_));
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Gun/classic.bin", meshes_, materials_, textures_)); //1
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Gun/SM_Bullet_01.bin", meshes_, materials_, textures_));
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Monster/Hit_Dragon.bin", meshes_, materials_, textures_));
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Monster/Shot_Dragon.bin", meshes_, materials_, textures_));
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Monster/Bomb_Dragon.bin", meshes_, materials_, textures_));
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Monster/Strong_Dragon.bin", meshes_, materials_, textures_));
	
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Gun/vandal.bin", meshes_, materials_, textures_));	//7 밴달
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Gun/odin.bin", meshes_, materials_, textures_));	//8 오딘
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Gun/flamethrower.bin", meshes_, materials_, textures_));	//9 화염방사기
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Gun/sherif.bin", meshes_, materials_, textures_));	//10 셰리프
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Gun/specter.bin", meshes_, materials_, textures_));	//11 스펙터
	
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Object/Chest.bin", meshes_, materials_, textures_));	//12 상자
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Object/Scroll.bin", meshes_, materials_, textures_));	//13 스크롤

	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Monster/Super_Dragon.bin", meshes_, materials_, textures_));

	// 씬 배치 정보 로딩
	std::ifstream scene_file{ "./Resource/Model/World/Scene.bin", std::ios::binary };

	int root_object_count = ReadFromFile<int>(scene_file);

	std::string load_token;

	for (int i = 0; i < root_object_count; ++i)
	{
		ReadStringFromFile(scene_file, load_token);

		if (load_token[0] == '@')
		{
			load_token.erase(0, 1);
			std::string object_name = load_token;
			object_list_.emplace_back();
			object_list_.back().reset(FindModelInfo(object_name)->GetInstance());
			object_list_.back()->set_name(object_name);

			ReadStringFromFile(scene_file, load_token);  // <Transform>
			XMFLOAT4X4 transform;
			if (load_token == "<SectorBounds>:")
			{
				auto bounds_center = ReadFromFile<XMFLOAT3>(scene_file);
				auto bounds_extents = ReadFromFile<XMFLOAT3>(scene_file);
				BoundingBox bounds{ bounds_center, bounds_extents };
				sectors_.emplace_back(object_name, bounds);
				ReadStringFromFile(scene_file, load_token); //<Transform>
				transform = ReadFromFile<XMFLOAT4X4>(scene_file);
				bounds.Transform(bounds, XMLoadFloat4x4(&transform));
				sectors_.emplace_back(object_name, bounds);
			}
			else
			{
				transform = ReadFromFile<XMFLOAT4X4>(scene_file);
			}
			object_list_.back()->set_transform_matrix(transform);

		}
		else
		{
			std::string object_name = load_token;  
			XMFLOAT4X4 transform;
			ReadStringFromFile(scene_file, load_token);
			bool is_sector = false;

			if (load_token == "<SectorBounds>:")
			{
				is_sector = true;
				auto bounds_center = ReadFromFile<XMFLOAT3>(scene_file);
				auto bounds_extents = ReadFromFile<XMFLOAT3>(scene_file);
				BoundingBox bounds{ bounds_center, bounds_extents };
				ReadStringFromFile(scene_file, load_token); //<Transform>
				transform = ReadFromFile<XMFLOAT4X4>(scene_file);
				bounds.Transform(bounds, XMLoadFloat4x4(&transform));
				sectors_.emplace_back(object_name, bounds);
			}
			else
			{
				transform = ReadFromFile<XMFLOAT4X4>(scene_file);
			}

			model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/World/" + object_name + ".bin", meshes_, materials_, textures_));

			object_list_.emplace_back();
			object_list_.back().reset(model_infos_.back()->GetInstance());
			object_list_.back()->set_name(object_name);

			object_list_.back()->set_transform_matrix(transform);
			if (is_sector)
			{
				sectors_.back().object_list().push_back(object_list_.back().get());
			}
		}

	}

	a_star::ConnectNodes(kNodeConnectors);

	kNodeConnectors.clear();
}
void BaseScene::BuildModelInfo()
{
	//Hit Dragon Fix(Add Hp UI, Set CollisionType)
	ModelInfo* hit_dragon = FindModelInfo("Hit_Dragon");
	hit_dragon->hierarchy_root()->set_collide_type(true, true);
	hit_dragon->hierarchy_root()->set_is_movable(true);
	hit_dragon->hierarchy_root()->set_tag("Hit_Dragon");

	//Shot Dragon Fix(Add Hp UI, Set CollisionType)
	ModelInfo* shot_dragon = FindModelInfo("Shot_Dragon");
	shot_dragon->hierarchy_root()->set_collide_type(true, true);
	shot_dragon->hierarchy_root()->set_is_movable(true);
	shot_dragon->hierarchy_root()->set_tag("Shot_Dragon");

	//Bomb Dragon Fix(Add Hp UI, Set CollisionType)
	ModelInfo* bomb_dragon = FindModelInfo("Bomb_Dragon");
	bomb_dragon->hierarchy_root()->set_collide_type(true, true);
	bomb_dragon->hierarchy_root()->set_is_movable(true);
	bomb_dragon->hierarchy_root()->set_tag("Bomb_Dragon");

	//Strong Dragon Fix(Set CollisionType)
	ModelInfo* strong_dragon = FindModelInfo("Strong_Dragon");
	strong_dragon->hierarchy_root()->set_collide_type(true, true);
	strong_dragon->hierarchy_root()->set_is_movable(true);
	strong_dragon->hierarchy_root()->set_tag("Strong_Dragon");

	//Create Hit Dragon Spawner
	ModelInfo* hit_dragon_spawner = new ModelInfo();
	hit_dragon_spawner->set_model_name("Hit_Dragon_Spawner");
	Object* spawner = new Object();
	auto monster_component = new MonsterComponent(nullptr);
	monster_component->set_scene(this);
	auto spawner_component = new SpawnerComponent(spawner, this, hit_dragon);
	spawner_component->AddComponent(monster_component);
	spawner_component->AddComponent(std::make_unique<MovementComponent>(nullptr));
	spawner_component->SetMonsterType(MonsterType::Hit_Dragon);
	spawner->AddComponent(spawner_component);
	hit_dragon_spawner->set_hierarchy_root(spawner);
	model_infos_.emplace_back();
	model_infos_.back().reset(hit_dragon_spawner);

	//Create Shot Dragon Spawner
	ModelInfo* shot_dragon_spawner = new ModelInfo();
	shot_dragon_spawner->set_model_name("Shot_Dragon_Spawner");
	spawner = new Object();
	spawner_component = new SpawnerComponent(spawner, this, shot_dragon);
	spawner_component->AddComponent(monster_component->GetCopy());
	spawner_component->AddComponent(std::make_unique<MovementComponent>(nullptr));
	spawner_component->SetMonsterType(MonsterType::Shot_Dragon);
	spawner->AddComponent(spawner_component);
	shot_dragon_spawner->set_hierarchy_root(spawner);
	model_infos_.emplace_back();
	model_infos_.back().reset(shot_dragon_spawner);

	//Create Bomb Dragon Spawner
	ModelInfo* bomb_dragon_spawner = new ModelInfo();
	bomb_dragon_spawner->set_model_name("Bomb_Dragon_Spawner");
	spawner = new Object();
	spawner_component = new SpawnerComponent(spawner, this, bomb_dragon);
	spawner_component->AddComponent(monster_component->GetCopy());
	spawner_component->AddComponent(std::make_unique<MovementComponent>(nullptr));
	spawner_component->SetMonsterType(MonsterType::Bomb_Dragon);
	spawner->AddComponent(spawner_component);
	bomb_dragon_spawner->set_hierarchy_root(spawner);
	model_infos_.emplace_back();
	model_infos_.back().reset(bomb_dragon_spawner);

	//Create Strong Dragon Spawner
	ModelInfo* strong_dragon_spawner = new ModelInfo();
	strong_dragon_spawner->set_model_name("Strong_Dragon_Spawner");
	spawner = new Object();
	spawner_component = new SpawnerComponent(spawner, this, strong_dragon);
	spawner_component->AddComponent(monster_component->GetCopy());
	spawner_component->AddComponent(std::make_unique<MovementComponent>(nullptr));
	spawner_component->SetMonsterType(MonsterType::Strong_Dragon);
	spawner->AddComponent(spawner_component);
	strong_dragon_spawner->set_hierarchy_root(spawner);
	model_infos_.emplace_back();
	model_infos_.back().reset(strong_dragon_spawner);

	//Create Gun Models
	{
		//Create Classic Model 1
		{
			ModelInfo* classic_model = new ModelInfo();
			auto classic_object = model_infos_[1]->GetInstance();
			classic_object->set_name("Classic");
			classic_model->set_hierarchy_root(classic_object);
			classic_model->set_model_name("Classic");

			GunComponent* gun_component = new GunComponent(classic_object);
			gun_component->LoadGunInfo("classic");
			classic_object->AddComponent(gun_component);
			classic_object->Rotate(0, 170, -17);

			model_infos_.emplace_back();
			model_infos_.back().reset(classic_model);
		}
		//Create vandal Model 7
		{
			ModelInfo* vandal_model = new ModelInfo();
			auto vandal_object = model_infos_[7]->GetInstance();
			vandal_object->set_name("Vandal");
			vandal_model->set_hierarchy_root(vandal_object);
			vandal_model->set_model_name("Vandal");

			GunComponent* gun_component = new GunComponent(vandal_object);
			gun_component->LoadGunInfo("vandal");
			vandal_object->AddComponent(gun_component);
			vandal_object->Rotate(0, 170, -17);

			model_infos_.emplace_back();
			model_infos_.back().reset(vandal_model);
		}
		//Create odin Model 8
		{
			ModelInfo* odin_model = new ModelInfo();
			auto odin_object = model_infos_[8]->GetInstance();
			odin_object->set_name("Odin");
			odin_model->set_hierarchy_root(odin_object);
			odin_model->set_model_name("Odin");

			GunComponent* gun_component = new GunComponent(odin_object);
			gun_component->LoadGunInfo("odin");
			odin_object->AddComponent(gun_component);
			odin_object->Rotate(0, 170, -17);

			model_infos_.emplace_back();
			model_infos_.back().reset(odin_model);
		}
		//Create flamethrower model 9
		{
			ModelInfo* flamethrower_model = new ModelInfo();
			auto flamethrower_object = model_infos_[9]->GetInstance();
			flamethrower_object->set_name("Flamethrower");
			flamethrower_model->set_hierarchy_root(flamethrower_object);
			flamethrower_model->set_model_name("Flamethrower");

			GunComponent* gun_component = new GunComponent(flamethrower_object);
			gun_component->LoadGunInfo("flamethrower");
			flamethrower_object->AddComponent(gun_component);
			flamethrower_object->Rotate(0, 170, -17);

			Object* player_gun_particle_pivot = new Object("gun_particle_pivot");
			flamethrower_object->AddChild(player_gun_particle_pivot);
			player_gun_particle_pivot->set_local_position(XMFLOAT3(0.0f, 0.143f, 1.24f));

			//// 화염방사기에 충돌 박스 달기
			auto flamethrow_box_component = new BoxColliderComponent(player_gun_particle_pivot, gun_component->flamethrow_box());
			player_gun_particle_pivot->AddComponent(flamethrow_box_component);

			model_infos_.emplace_back();
			model_infos_.back().reset(flamethrower_model);
		}
		//Create sherif Model 10
		{
			ModelInfo* sherif_model = new ModelInfo();
			auto sherif_object = model_infos_[10]->GetInstance();
			sherif_object->set_name("Sherif");
			sherif_model->set_hierarchy_root(sherif_object);
			sherif_model->set_model_name("Sherif");

			GunComponent* gun_component = new GunComponent(sherif_object);
			gun_component->LoadGunInfo("sherif");
			sherif_object->AddComponent(gun_component);
			sherif_object->Rotate(0, 170, -17);

			model_infos_.emplace_back();
			model_infos_.back().reset(sherif_model);
		}
		//Create specter Model 11
		{
			ModelInfo* specter_model = new ModelInfo();
			auto specter_object = model_infos_[11]->GetInstance();
			specter_object->set_name("Specter");
			specter_model->set_hierarchy_root(specter_object);
			specter_model->set_model_name("Specter");

			GunComponent* gun_component = new GunComponent(specter_object);
			gun_component->LoadGunInfo("specter");
			specter_object->AddComponent(gun_component);
			specter_object->Rotate(0, 170, -17);

			model_infos_.emplace_back();
			model_infos_.back().reset(specter_model);
		}
	}


	//Fix Scroll(Add Scroll UI)
	{
		const int scroll_model_index = 13;
		const int scroll_num = 10;

		for (int i = 0; i < scroll_num; ++i)
		{
			// 스크롤 오브젝트 생성
			ModelInfo* scroll_model_info = new ModelInfo();
			scroll_model_info->set_model_name("Scroll_" + std::to_string(i));
			model_infos_.emplace_back();
			model_infos_.back().reset(scroll_model_info);

			Object* scroll = model_infos_[scroll_model_index]->GetInstance();
			scroll_model_info->set_hierarchy_root(scroll);
			scroll->set_name("Scroll_" + std::to_string(i));
			scroll->set_tag("Scroll");
			scroll->set_is_movable(true);

			auto scroll_component = new ScrollComponent(scroll);
			scroll_component->set_type(static_cast<ScrollType>(i));
			scroll->AddComponent(scroll_component);

			//scroll->set_local_rotation(scroll_rotations[0]);

			Object* ui_bar = new Object();
			ui_bar->set_tag("Scroll_UI");

			// 메쉬 및 머티리얼 설정
			Mesh* mesh = Scene::FindMesh("Scroll", meshes_);
			std::string material_name = "scroll_material_" + std::to_string(i); // 인덱스별
			Material* material = Scene::FindMaterial(material_name, materials_);
		}
	}
}

void BaseScene::BuildObject()
{

	constexpr int kSpawnBoxCount = 5;
	spawn_boxs_.reserve(kSpawnBoxCount);
	for (int i = 0; i < kSpawnBoxCount; ++i)
	{
		const auto& box_object = FindObject("SpawnBox" + std::to_string(i + 1));
		//if (box_object) {
		//	std::cout << box_object->name() << std::endl;
		//	std::cout << box_object->position_vector().x << std::endl;
		//	std::cout << box_object->position_vector().y << std::endl;
		//	std::cout << box_object->position_vector().y << std::endl;
		//}
		const auto& box = Object::GetComponent<BoxColliderComponent>(box_object);
		
		spawn_boxs_.push_back(box);
	}
	// 스테이지3 클리어 트리거 박스 생성
	{
		XMFLOAT3 box_pos = { 63.25f, 0.98f, -113.28f };
		XMFLOAT3 box_scale = { 7.1f, 1.9f, 5.3f };
		stage3_clear_box_.Center = box_pos;
		stage3_clear_box_.Extents = box_scale;
		stage3_clear_box_.Orientation = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
	}


	Mesh* debug_mesh = Scene::FindMesh("Debug_Mesh", meshes_);
	const auto& const debug_material = Scene::FindMaterial("debug", materials_);

	std::cout << "[DEBUG] 전체 오브젝트 수: " << object_list_.size() << std::endl;

	for (auto& object_ptr : object_list_)
	{
		Object* object = object_ptr.get();
		// 재귀적으로 MeshComponent를 찾아서 처리
		auto mesh_components = Object::GetComponentsInChildren<MeshComponent>(object);
		std::cout << "[DEBUG] Object name: " << object->name()
			<< "  MeshComponents: " << mesh_components.size() << std::endl;

		for (auto& mesh_comp : mesh_components)
		{
			auto mesh = mesh_comp->GetMesh();

			if (!mesh)
			{
				std::cout << "[ERROR] MeshComponent 에 Mesh 가 nullptr!!" << std::endl;
			}

			Object* object = mesh_comp->owner();
			MeshColliderComponent* mesh_collider = new MeshColliderComponent(object);
			mesh_collider->set_mesh(mesh);
			object->AddComponent(mesh_collider);
			if (mesh->name() != "Debug_Mesh")
			{
				if (debug_material)
				{
					auto debug_mesh_component = new DebugMeshComponent(object, debug_mesh, mesh->bounds());
					debug_mesh_component->AddMaterial(debug_material);
					object->AddComponent(debug_mesh_component);
				}
			}
		}
	}

	//Load All GunInfos
	GunComponent::LoadGunInfosFromFile("./Resource/GunInfos.txt");

	BuildModelInfo();

	PrepareGroundChecking();

	CreateMonsterSpawner();

	// 보물상자
	{
		constexpr int kChestCount = 6;
		XMFLOAT3 chest_positions[kChestCount] = {
			{ 32.19f, 0.38f, 8.44f },
			{ 101.82f, 0.38f, 16.34f },
			{ 56.3f, 0.147f, -108.4f },
			{ 65.9f, 0.351f, -183.7f },
			{ 110.72f, 0.346f, -175.84f},
			{ 141.55f, 0.35f, -164.17f }
		};
		XMFLOAT3 chest_rotations[kChestCount] = {
		{ 0.0f, 171.0f, 0.0f },
		{ 0.0f, 294.6f, 0.0f },
		{ 0.0f, 383.8f, 0.0f },
		{ 0.0f, 361.4f, 0.0f },
		{ 0.0f, 299.42f, 0.0f },
		{ 0.0f, 179.44f, 0.0f }
		};

		chests_.reserve(kChestCount);

		// 스크롤 인덱스 무작위화
		std::vector<int> scroll_index(10);
		std::iota(scroll_index.begin(), scroll_index.end(), 0); // 0~9 채우기
		std::shuffle(scroll_index.begin(), scroll_index.end(), kRandomGenerator);

		//테스트용
		/*scroll_index = {
			(int)ScrollType::kNinja,
			(int)ScrollType::kSprinter,
			(int)ScrollType::kWeaponMaster,
			(int)ScrollType::kFlameMaster,
			(int)ScrollType::kAcidMaster,
			(int)ScrollType::kElectricMaster
		};*/

		for (int i = 0; i < kChestCount; ++i)
		{
			Object* chest = model_infos_[12]->GetInstance();
			chest->set_name("Chest" + std::to_string(i));
			chest->set_position_vector(chest_positions[i]);
			chest->set_local_rotation(chest_rotations[i]);
			chest->set_is_movable(true);

			//스크롤 추가
			auto chest_component = new ChestComponent(chest, this);
			//박스당 1개 사용
			auto scroll_model = FindModelInfo("Scroll_" + std::to_string(scroll_index[i]));
			if (!scroll_model) std::cout << "스크롤 모델을 찾기 못했습니다." << std::endl;
			chest_component->set_scroll_model(scroll_model);
			chest->AddComponent(chest_component);

			// 충돌용 BoxColliderComponent 부착
			BoundingBox box_bounds{ {0.0f, 0.0f, 0.0f}, {1.5f, 1.0f, 1.5f} };
			auto collider = new BoxColliderComponent(chest, box_bounds);
			chest->AddComponent(collider);


			AddObject(chest);
			chests_.push_back(chest);

		}
	}

	////Create Razer Model
	//{
	//	ModelInfo* razer_model = new ModelInfo();
	//	auto razer_object = new Object("Razer");
	//	auto razer_component = new RazerComponent(razer_object);
	//	auto mesh_component = new BillboardMeshComponent(razer_object,
	//		FindMesh("RazerMesh", meshes_), FindMaterial("Razer", materials_), this);
	//	FindMaterial("Razer", materials_)->DeleteMeshComponent(mesh_component);
	//	razer_object->AddComponent(mesh_component);
	//	razer_object->AddComponent(razer_component);
	//	razer_object->set_is_movable(true);
	//	razer_model->set_hierarchy_root(razer_object);
	//	razer_model->set_model_name("Razer");
	//	model_infos_.emplace_back();
	//	model_infos_.back().reset(razer_model);
	//}

	{
		ModelInfo* razer_model = new ModelInfo();
		auto razer_object = new Object("Razer");
		auto razer_component = new RazerComponent(razer_object);
		auto mesh_component = new BillboardMeshComponent(
			razer_object,
			FindMesh("RazerMesh", meshes_),
			nullptr, // 재질 제거됨
			this
		);
		// 재질 관련 코드 제거됨:
		//FindMaterial("Razer", materials_)->DeleteMeshComponent(mesh_component);

		razer_object->AddComponent(mesh_component);
		razer_object->AddComponent(razer_component);
		razer_object->set_is_movable(true);
		razer_model->set_hierarchy_root(razer_object);
		razer_model->set_model_name("Razer");
		model_infos_.emplace_back();
		model_infos_.back().reset(razer_model);
	}

	catch_monster_num_ = 1;


	//std::cout << "========== DEBUG COLLIDER CHECK ==========\n";
	//for (const auto& obj : object_list_)
	//{
	//	auto mesh_collider_list = Object::GetComponentsInChildren<MeshColliderComponent>(obj.get());
	//	std::cout << "[Object] " << obj->name()
	//		<< " → " << mesh_collider_list.size() << " MeshCollider(s)" << std::endl;
	//}
	//
	//std::cout << "STAGE1 object: " << (Scene::FindObject("STAGE1") ? "Found" : "Not Found") << std::endl;
}

void BaseScene::CreateMonsterSpawner()
{
	std::function<Object* (ModelInfo*, int&, XMFLOAT3, int, float, float)> create_spawner =
		[this]
	(ModelInfo* spawner_model, int& spawner_id, XMFLOAT3 spawn_position, int spawn_count, float spawn_time, float spawn_cool_time)
	{
		Object* spawner = spawner_model->GetInstance();
		spawner->set_name(spawner_model->model_name() + "_" + std::to_string(++spawner_id));
		spawner->set_position_vector(spawn_position);
		auto spawner_component = Object::GetComponent<SpawnerComponent>(spawner);
		spawner_component->SetSpawnerInfo(spawn_count, spawn_time, spawn_cool_time);
	
		return spawner;
	};
	
	//hit dragon
	int hit_spawner_id = 0;
	ModelInfo* hit_dragon_spawner = FindModelInfo("Hit_Dragon_Spawner");
	
	//shot dragon
	int shot_spawner_id = 0;
	ModelInfo* shot_dragon_spawner = FindModelInfo("Shot_Dragon_Spawner");
	
	//bomb dragon
	int bomb_spawner_id = 0;
	ModelInfo* bomb_dragon_spawner = FindModelInfo("Bomb_Dragon_Spawner");
	
	//strong dragon
	int strong_spawner_id = 0;
	ModelInfo* strong_dragon_spawner = FindModelInfo("Strong_Dragon_Spawner");
	
	Object* spawner;
	SpawnerComponent* spawner_component;
	//Stage 1
	{
		spawner = create_spawner(hit_dragon_spawner, hit_spawner_id, XMFLOAT3{ 17.38f, 0.61f, -0.92f }, 3, 3.f, 5.f);
		spawner_component = Object::GetComponent<SpawnerComponent>(spawner);
		AddObject(spawner);
		stage_monster_spawner_list_[0].push_back(spawner_component);
		
		spawner = create_spawner(hit_dragon_spawner, hit_spawner_id, XMFLOAT3{ 16.f, 2.6f, 11.74f }, 3, 4.f, 4.f);
		spawner_component = Object::GetComponent<SpawnerComponent>(spawner);
		AddObject(spawner);
		stage_monster_spawner_list_[0].push_back(spawner_component);
		
		spawner = create_spawner(hit_dragon_spawner, hit_spawner_id, XMFLOAT3{ 16.84f, 1.24f, -9.07f }, 3, 5.f, 3.f);
		spawner_component = Object::GetComponent<SpawnerComponent>(spawner);
		AddObject(spawner);
		stage_monster_spawner_list_[0].push_back(spawner_component);
		
		spawner = create_spawner(shot_dragon_spawner, shot_spawner_id, XMFLOAT3{ 27.85f, 6.73f, -8.07f }, 1, 9.f, 5.f);
		spawner_component = Object::GetComponent<SpawnerComponent>(spawner);
		AddObject(spawner);
		stage_monster_spawner_list_[0].push_back(spawner_component);
		
		spawner = create_spawner(shot_dragon_spawner, shot_spawner_id, XMFLOAT3{ 24.53f, 5.31f, 10.05f }, 1, 11.f, 5.f);
		spawner_component = Object::GetComponent<SpawnerComponent>(spawner);
		AddObject(spawner);
		stage_monster_spawner_list_[0].push_back(spawner_component);
	}
	
	//Stage 2
	{
		//hit 1
		spawner = create_spawner(hit_dragon_spawner, hit_spawner_id, XMFLOAT3{ 58.91f, 2.97f, 0.28f }, 3, 0.5f, 5.f);
		spawner_component = Object::GetComponent<SpawnerComponent>(spawner);
		AddObject(spawner);
		stage_monster_spawner_list_[1].push_back(spawner_component);
	
		//hit 2		
		spawner = create_spawner(hit_dragon_spawner, hit_spawner_id, XMFLOAT3{ 58.91f, 2.97f, 9.13f }, 3, 0.5f, 5.f);
		spawner_component = Object::GetComponent<SpawnerComponent>(spawner);
		AddObject(spawner);
		stage_monster_spawner_list_[1].push_back(spawner_component);
	
		//shot 1
		spawner = create_spawner(shot_dragon_spawner, shot_spawner_id, XMFLOAT3{ 63.63f, 7.66f, -3.49f }, 1, 3.f, 5.f);
		spawner_component = Object::GetComponent<SpawnerComponent>(spawner);
		AddObject(spawner);
		stage_monster_spawner_list_[1].push_back(spawner_component);
	
		//shot 2
		spawner = create_spawner(shot_dragon_spawner, shot_spawner_id, XMFLOAT3{ 63.63f, 7.66f, 11.94f }, 1, 3.f, 5.f);
		spawner_component = Object::GetComponent<SpawnerComponent>(spawner);
		AddObject(spawner);
		stage_monster_spawner_list_[1].push_back(spawner_component);
	
		//shot 3
		spawner = create_spawner(shot_dragon_spawner, shot_spawner_id, XMFLOAT3{ 80.41f, 7.66f, 11.94f }, 1, 8.f, 5.f);
		spawner_component = Object::GetComponent<SpawnerComponent>(spawner);
		AddObject(spawner);
		stage_monster_spawner_list_[1].push_back(spawner_component);
	
		//shot 4
		spawner = create_spawner(shot_dragon_spawner, shot_spawner_id, XMFLOAT3{ 80.41f, 7.66f, -3.45f }, 1, 8.f, 5.f);
		spawner_component = Object::GetComponent<SpawnerComponent>(spawner);
		AddObject(spawner);
		stage_monster_spawner_list_[1].push_back(spawner_component);
	
		////bomb 1
		//spawner = create_spawner(bomb_dragon_spawner, bomb_spawner_id, XMFLOAT3{ 50.f, 0.47f, 24.14f }, 2, 14.f, 4.f);
		//spawner_component = Object::GetComponent<SpawnerComponent>(spawner);
		//AddObject(spawner);
		//stage_monster_spawner_list_[1].push_back(spawner_component);
		//
		////bomb 2
		//spawner = create_spawner(bomb_dragon_spawner, bomb_spawner_id, XMFLOAT3{ 49.43f, 0.47f, -15.51f }, 2, 14.f, 4.f);
		//spawner_component = Object::GetComponent<SpawnerComponent>(spawner);
		//AddObject(spawner);
		//stage_monster_spawner_list_[1].push_back(spawner_component);
	}
	
	//Stage 4
	{
		spawner = create_spawner(strong_dragon_spawner, strong_spawner_id, XMFLOAT3{ 55.36f, 1.2f, -156.52f }, 1, 0.f, 4.f);
		spawner_component = Object::GetComponent<SpawnerComponent>(spawner);
		AddObject(spawner);
		stage_monster_spawner_list_[3].push_back(spawner_component);
	}
}

void BaseScene::ActivateStageMonsterSpawner(int stage_num)
{
	if (stage_num < 0 || stage_num >= kStageMaxCount)
	{
		return;
	}
	for (auto& spawner : stage_monster_spawner_list_[stage_num])
	{
		spawner->ActivateSpawn();
	}
}

void BaseScene::add_catch_monster_num()
{
	++catch_monster_num_;
}

const std::list<MonsterComponent*>& BaseScene::monster_list() const
{
	return monster_list_;
}

bool BaseScene::ProcessInput(void* p)
{
	//if (main_input_controller_)
	//{
	//	if (main_input_controller_->ProcessInput(id, w_param, l_param, time))
	//		return true;
	//}
	return false;
}

void BaseScene::Update(float elapsed_time)
{	
	Scene::Update(elapsed_time);

	UpdateObjectHitWall();

	UpdateObjectWorldMatrix();

	UpdateObjectIsGround();

	UpdateObjectHitBullet();

	UpdateObjectHitObject();

	UpdateRazerHitEnemy();

	DeleteDeadObjects();

	UpdateStageClear();

	CheckSpawnBoxHitPlayers();

	CheckPlayerHitChest();
}

Object* BaseScene::CreateAndRegisterPlayer(long long session_id)
{
	Object* player = model_infos_[0]->GetInstance();
	player->set_name("Player_" + std::to_string(session_id));
	player->set_position_vector(XMFLOAT3{ -15, 6, 0 });
	player->set_collide_type(true, true);  // 지면 & 벽 충돌 체크 등록
	player->set_is_movable(true);
	player->set_is_player();
	player->AddComponent(new MovementComponent(player));

	// 애니메이션 세팅
	if (auto animator = Object::GetComponent<AnimatorComponent>(player))
		animator->set_animation_state(new PlayerAnimationState);

	// 총기 부착
	Object* weapon_frame = player->FindFrame("WeaponR_locator");
	if (weapon_frame) {
		weapon_frame->AddChild(FindModelInfo("Flamethrower")->GetInstance());  // 총 모델
		weapon_frame = weapon_frame->child();  // 자식 프레임 가져옴

	}
	//else {
	//	std::cerr << "[DEBUG] WeaponR_locator not found." << std::endl;
	//}

	auto mesh_component_list = Object::GetComponentsInChildren<MeshComponent>(player);
	for (auto& mesh_component : mesh_component_list)
	{
		auto mesh = mesh_component->GetMesh();
		Object* object = mesh_component->owner();
		MeshColliderComponent* mesh_collider = new MeshColliderComponent(object);
		mesh_collider->set_mesh(mesh);
		object->AddComponent(mesh_collider);
	}

	auto player_component = new PlayerComponent(player);
	player_component->set_scene(this);
	player->AddComponent(player_component);

	AddObject(player);

	return player;
}


void BaseScene::AddObject(Object* object)
{
	Scene::AddObject(object);

	CollideType collide_type = object->collide_type();

	if (collide_type.ground_check)
	{
		ground_check_object_list_.push_back(object);
		//std::cout << "추가 완료" << std::endl;
	}
	if (collide_type.wall_check)
	{
		auto movement = Object::GetComponentInChildren<MovementComponent>(object);
		if (movement) {
			wall_check_object_list_.emplace_back(object, movement);
			//std::cout << "추가 완료" << std::endl;
		}

	}

	auto monster_component = Object::GetComponent<MonsterComponent>(object);
	if (monster_component)
	{
		monster_list_.push_back(monster_component);
	}

	auto razer_component = Object::GetComponent<RazerComponent>(object);
	if (razer_component)
	{
		razer_list_.push_back(razer_component);
	}
}

void BaseScene::DeleteObject(Object* object)
{
	CollideType collide_type = object->collide_type();
	if (collide_type.ground_check)
	{
		ground_check_object_list_.remove(object);
	}
	if (collide_type.wall_check)
	{
		wall_check_object_list_.remove_if([object](const WallCheckObject& wall_check_object) {
			return wall_check_object.object == object;
			});
	}

	Scene::DeleteObject(object);
}

void BaseScene::DeleteDeadObjects()
{
	monster_list_.remove_if([](const MonsterComponent* monster_component) {
		return monster_component->owner()->is_dead();
		});

	ground_check_object_list_.remove_if([](const Object* object) {
		return object->is_dead();
		});
	wall_check_object_list_.remove_if([](const WallCheckObject& wall_check_object) {
		return wall_check_object.object->is_dead();
		});
	Scene::DeleteDeadObjects();
}

void BaseScene::UpdateObjectIsGround()
{

	if (!is_prepare_ground_checking_)
	{
		PrepareGroundChecking();
	}

	for (auto& object : ground_check_object_list_)
	{
		if (!object)
		{
			std::cout << "[DEBUG] CheckObjectIsGround: object is nullptr" << std::endl;
			return;
		}

		CheckObjectIsGround(object);
	}
}

void BaseScene::UpdateObjectHitWall()
{
	if (!is_prepare_ground_checking_)
	{
		PrepareGroundChecking();
	}

	for (auto& wall_check_object : wall_check_object_list_)
	{
		CheckPlayerHitWall(wall_check_object.object, wall_check_object.movement);
	}
}

void BaseScene::UpdateObjectHitBullet()
{
	if (!is_prepare_ground_checking_)
	{
		PrepareGroundChecking();
	}
	const auto& users = SessionManager::getInstance().getAllSessions();
	for (const auto& player : users)
	{
		const auto& player_object = player.second->get_player_object();
		CheckPlayerHitGun(player_object);
		if (!player.second->is_firekey_down()) continue;
		GunComponent* gun = Object::GetComponentInChildren<GunComponent>(player_object);
		if (gun && gun->gun_name() == "flamethrower")
		{
			auto bullet_mesh = FindModelInfo("SM_Bullet_01")->GetInstance();
			gun->FireBullet(XMFLOAT3{}, bullet_mesh, this, player.second->get_id());
		}
		for (const auto& monster : monster_list_)
		{
			CheckObjectHitFlamethrow(monster->owner(), player.second->get_id());
		}
	}
}

void BaseScene::UpdateObjectHitObject()
{
	if (!is_prepare_ground_checking_)
	{
		PrepareGroundChecking();
	}
	for (auto& object : ground_check_object_list_)
	{
		auto movement = Object::GetComponentInChildren<MovementComponent>(object);
		CheckObjectHitObject(object);
	}
}

void BaseScene::UpdateStageClear()
{

	if (!is_prepare_ground_checking_)
	{
		PrepareGroundChecking();
	}
	switch (stage_clear_num_)
	{
	case 0:
		if (catch_monster_num_ <= 0)
			return;
		break;
	case 1:
		if (catch_monster_num_ < 11)
			return;
		break;
	case 2:
		if (catch_monster_num_ < 10)
			return;
		break;
	case 3:
	{	for (auto& object : ground_check_object_list_){
		if (!object->is_player()) return;
		auto player_collider = Object::GetComponentInChildren<MeshColliderComponent>(object);
		if (!player_collider) return;

		BoundingOrientedBox player_box = player_collider->GetWorldOBB();
		if (!stage3_clear_box_.Intersects(player_box))
			return;
	}
	}
	
	break;
	case 4:
		if (catch_monster_num_ < 1)
			return;
		break;
	case 5:
		if (catch_monster_num_ < 10)
			return;
		break;
	case 6:
		for (auto& object : ground_check_object_list_)
		{
			auto movement = Object::GetComponentInChildren<MovementComponent>(object);
			CheckPlayerHitPyramid(object);
		}
		if (get_key_num_ == 3)
		{
			auto& mesh_list = stage_wall_collider_list_[stage_clear_num_];
			mesh_list.remove_if([](MeshColliderComponent* collider) {
				return collider->mesh() && collider->mesh()->name() == "Cube";
				});
			++stage_clear_num_;
			get_key_num_ = 0;
		}
		break;
	case 7:
		// TODO: 게임클리어!
		if (catch_monster_num_ < 1)
			return;
		break;
	default:
		break;
	}

	// 현재 스테이지에서 "Cube" 메쉬 제거
	auto& mesh_list = stage_wall_collider_list_[stage_clear_num_];
	mesh_list.remove_if([](MeshColliderComponent* collider) {
		return collider->mesh() && collider->mesh()->name() == "Cube";
		});

	std::cout << "현재 스테이지: " << stage_clear_num_ << std::endl;
	std::cout << "잡은 몬스터 수: " << catch_monster_num_ << std::endl;

	// 스테이지 넘버 증가
	++stage_clear_num_;
	catch_monster_num_ = 0;

	std::cout << "현재 스테이지: " << stage_clear_num_ << std::endl;
	std::cout << "잡은 몬스터 수: " << catch_monster_num_ << std::endl;

	// 스테이지 넘버 증가 패킷 전송
	sc_packet_stage_clear sc;
	sc.size = sizeof(sc_packet_stage_clear);
	sc.stage_num = stage_clear_num_;
	sc.type = S2C_P_STAGE_CLEAR;

	const auto& users = SessionManager::getInstance().getAllSessions();
	for (auto& u : users) {
		u.second->do_send(&sc);
	}

	is_activate_spawner_ = false;
}

void BaseScene::PrepareGroundChecking()
{
	static const std::array<std::string, kStageMaxCount>
		stage_names{ "BASE", "STAGE1", "STAGE2", "STAGE3", "STAGE4", "STAGE5", "STAGE6", "STAGE7" };
	for (int i = 0; i < stage_names.size(); ++i)
	{
		Object* object = Scene::FindObject(stage_names[i]);
		//std::cout << object->name() << std::endl;
		checking_maps_mesh_collider_list_[i] = Object::GetComponentsInChildren<MeshColliderComponent>(object);
		stage_ground_collider_list_[i] = Object::GetComponentsInChildren<GroundColliderComponent>(object);
		stage_wall_collider_list_[i] = Object::GetComponentsInChildren<WallColliderComponent>(object);
	}
	is_prepare_ground_checking_ = true;
}

void BaseScene::CheckPlayerHitGun(Object* object)
{
	Session* session = SessionManager::getInstance().GetSessionByPlayerObject(object);
	if (!session)
		return;

	auto player_box = Object::GetComponentInChildren<MeshColliderComponent>(object);
	if (!player_box) return;

	BoundingOrientedBox player_obb = player_box->GetWorldOBB();

	for (auto it = dropped_guns_.begin(); it != dropped_guns_.end(); )
	{
		Object* gun = *it;
		auto gun_box = Object::GetComponent<BoxColliderComponent>(gun);
		if (!gun_box) { ++it; continue; }

		// F 키 입력된 세션만 판정
		if (player_obb.Intersects(gun_box->animated_box()) && session->IsKeyDown('F'))
		{
			GunComponent* gun_component = Object::GetComponent<GunComponent>(gun);
			if (!gun_component) { ++it; continue; }

			std::string dropped_name = gun->name(); // "Dropped_Classic"
			std::string gun_name = dropped_name.substr(dropped_name.find('_') + 1); // "Classic"


			int upgrade = gun_component->upgrade();
			ElementType element = gun_component->element();

			Object* player_gun_frame = object->FindFrame("WeaponR_locator");
			if (!player_gun_frame) { ++it; continue; }

			std::vector<std::string> guns{ "Classic", "Sherif", "Specter", "Vandal", "Odin", "Flamethrower" };
			for (const auto& name : guns)
			{
				if (name == gun_name) continue;
				player_gun_frame->ChangeChild(FindModelInfo(gun_name)->GetInstance(), name, true);
			}

			Object* new_gun = player_gun_frame->FindFrame(gun_name);
			GunComponent* new_gun_component = Object::GetComponent<GunComponent>(new_gun);
			if (new_gun_component)
			{
				new_gun_component->set_upgrade(upgrade);
				new_gun_component->set_element(element);
			}

			// 교체 패킷 전송
			sc_packet_gun_change gc;
			gc.size = sizeof(gc);
			gc.type = S2C_P_GUN_CHANGE;
			gc.id = session->get_id(); // 클라이언트에서 어떤 플레이어에 해당하는지 식별
			gc.gun_id = gun_component->owner()->id();
			std::cout << "교체 총 id: " << gun_component->owner()->id() << std::endl;
			strcpy_s(gc.gun_name, gun_name.c_str());
			gc.upgrade_level = gun_component->upgrade();
			gc.element_type = static_cast<int>(gun_component->element());

			const auto& users = SessionManager::getInstance().getAllSessions();
			for (const auto& user : users) {
				user.second->do_send(&gc);
			}

			gun->set_is_dead(true);
			it = dropped_guns_.erase(it);

			// F 키를 한 번만 사용하도록 해제
			session->SetKeyDown('F', false);
		}
		else
		{
			++it;
		}
	}

}

void BaseScene::UpdateRazerHitEnemy()
{
	for (auto& razer : razer_list_)
	{
		for (auto& monster : monster_list_)
		{
			CheckRazerHitEnemy(razer, monster);
		}
	}
}

void BaseScene::CheckObjectIsGround(Object* object)
{
	//std::cout << "[DEBUG] Entered CheckObjectIsGround()" << std::endl;

	XMFLOAT3 position = object->world_position_vector();
	constexpr float kGroundYOffset = 0.75f;
	position.y += kGroundYOffset;
	XMVECTOR ray_origin = XMLoadFloat3(&position);
	position.y -= kGroundYOffset;
	XMVECTOR ray_direction = XMVectorSet(0, -1, 0, 0);

	bool is_collide = false;
	float distance{ std::numeric_limits<float>::max() };
	for (auto& mesh_collider : stage_ground_collider_list_[stage_clear_num_])
	{
		//++a;
		float t{};
		if (mesh_collider->CollisionCheckByRay(ray_origin, ray_direction, t))
		{
			is_collide = true;
			if (t < distance)
			{
				distance = t;
			}
		}
	}
	if (stage_clear_num_ - 1 >= 0)
	{
		for (auto& mesh_collider : stage_ground_collider_list_[stage_clear_num_ - 1])
		{
			//++a;
			float t{};
			if (mesh_collider->CollisionCheckByRay(ray_origin, ray_direction, t))
			{
				is_collide = true;
				if (t < distance)
				{
					distance = t;
				}
			}
		}
	}
	if (is_collide)
	{
		float distance_on_ground = distance - kGroundYOffset; //지면까지의 거리
		if (distance_on_ground > 0.005f)
		{
			object->set_is_ground(false);
			return;
		}
		position.y -= distance_on_ground;
		object->set_is_ground(true);
		object->set_position_vector(position);
		return;
	}

	object->set_is_ground(false);
}

void BaseScene::CheckPlayerHitWall(Object* object, MovementComponent* movement)
{
	XMFLOAT3 velocity = movement->velocity();

	XMFLOAT3 position = object->world_position_vector();
	constexpr float kGroundYOffset = 0.75f;
	position.y += kGroundYOffset;
	XMVECTOR ray_origin = XMLoadFloat3(&position);
	position.y -= kGroundYOffset;


	XMVECTOR ray_direction = XMLoadFloat3(&velocity);
	ray_direction = XMVectorSetY(ray_direction, 0);
	ray_direction = XMVector3Normalize(ray_direction);

	if (0 == XMVectorGetX(XMVector3Length(ray_direction))) {
		return;
	}

	bool is_collide = false;
	float distance{ std::numeric_limits<float>::max() };
	int a = 0;
	constexpr float MAX_DISTANCE = 0.5f;

	for (auto& mesh_collider : stage_wall_collider_list_[stage_clear_num_])
	{
		++a;
		float t{};
		if (mesh_collider->CollisionCheckByRay(ray_origin, ray_direction, t))
		{
			if (t < distance)
			{
				distance = t;
			}
		}
	}
	if (stage_clear_num_ - 1 >= 0)
	{
		for (auto& mesh_collider : stage_wall_collider_list_[stage_clear_num_ - 1])
		{
			++a;
			float t{};
			if (mesh_collider->CollisionCheckByRay(ray_origin, ray_direction, t))
			{
				if (t < distance)
				{
					distance = t;
				}
				
			}
		}
	}
	if (distance < MAX_DISTANCE)
		is_collide = true;

	//OutputDebugString(std::wstring(L"MeshColliderComponent Count: " + std::to_wstring(a) + L"\n").c_str());

	if (is_collide)
	{
		movement->Stop();
		return;
	}

}

void BaseScene::CheckObjectHitObject(Object* object)
{
	//TODO: 몬스터 AI완성 이후 충돌시에 밀리는 기능 추가 및 return; 삭제!!
	return;


	if (!object || object->is_dead()) return;

	auto movement = Object::GetComponentInChildren<MovementComponent>(object);
	if (!movement) return;

	XMFLOAT3 object_pos = object->world_position_vector();

	auto mesh_collider = Object::GetComponentInChildren<MeshColliderComponent>(object);
	if (mesh_collider)
	{
		BoundingOrientedBox obb1 = mesh_collider->GetWorldOBB();

		for (auto& other : ground_check_object_list_)
		{
			if (!other || other == object || other->is_dead()) continue;

			auto other_box = Object::GetComponentInChildren<BoxColliderComponent>(other);
			if (!other_box) continue;

			if (obb1.Intersects(other_box->animated_box()))
			{
				XMFLOAT3 position = object->world_position_vector();
				constexpr float kGroundYOffset = 0.75f;
				position.y += kGroundYOffset;
				XMVECTOR ray_origin = XMLoadFloat3(&position);
				position.y -= kGroundYOffset;

				XMFLOAT3 other_pos = other->world_position_vector();
				XMFLOAT3 dir = xmath_util_float3::Normalize(object_pos - other_pos);

				XMVECTOR ray_direction = XMLoadFloat3(&dir);
				ray_direction = XMVectorSetY(ray_direction, 0);
				ray_direction = XMVector3Normalize(ray_direction);

				if (0 == XMVectorGetX(XMVector3Length(ray_direction)))
					return;

				bool is_collide = false;
				float distance{ std::numeric_limits<float>::max() };
				for (auto& mesh_collider : stage_wall_collider_list_[stage_clear_num_])
				{
					float t{};
					if (mesh_collider->CollisionCheckByRay(ray_origin, ray_direction, t))
					{
						if (t < distance)
						{
							distance = t;
						}
					}
				}

				constexpr float kMinSafeDistance = 1.5f; // 살짝 밀려도 충돌 안나도록 여유
				if (distance > kMinSafeDistance) // 벽에 안 부딪힌다면 밀기
				{
					object->set_position_vector(object_pos + dir * 0.1f);

					// TODO : 몬스터 AI완성 이후 충돌시에 밀리는 기능 추가

					auto monster = Object::GetComponent<MonsterComponent>(object);
					if (monster)
					{
						monster->set_is_pushed(true);
						monster->set_push_timer(5.0f);
					}
				}
				return;
			}
		}
	}
}

void BaseScene::CheckObjectHitBullet(Object* object)
{
	auto box_collider_list = Object::GetComponentsInChildren<BoxColliderComponent>(object);
	if (box_collider_list.empty())
		return;

	const auto& users = SessionManager::getInstance().getAllSessions();
	for (const auto& pair : users)
	{
		const auto& user = pair.second;

		if (!user)
			continue;

		Object* player = user->get_player_object();
		if (!player)
			continue;

		GunComponent* gun = Object::GetComponentInChildren<GunComponent>(player);
		if (!gun)
			continue;

		auto bullet_list = gun->fired_bullet_list();
		for (auto& box_collider : box_collider_list)
		{
			for (auto& bullet : bullet_list)
			{
				if (bullet->is_dead())
					continue;

				BoxColliderComponent* bullet_collider = Object::GetComponent<BoxColliderComponent>(bullet);
				if (!bullet_collider)
					continue;

				if (bullet_collider->animated_box().Intersects(box_collider->animated_box()))
				{
					MonsterComponent* monster = Object::GetComponent<MonsterComponent>(object);
					if (monster && monster->IsDead())
						continue;

					bullet->set_is_dead(true);

					if (monster && !monster->IsDead())
					{
						monster->HitDamage(gun->damage() * (1 + gun->upgrade() * 0.2));

						if (monster->IsDead()) {
							// 총기 이름 목록
							std::vector<std::string> gun_names = { "Classic", "Sherif", "Specter", "Vandal", "Odin", "Flamethrower" };

							std::vector<int> drop_weights = { 15, 10, 7, 5, 3, 1 }; // 전체 합 = 41

							// 드랍할지 말지: 41% 확률로 총기 드랍, 나머지 59%는 아무것도 안 떨어짐
							if (rand() % 100 >= 41) return; // 59% 확률로 드랍 안 함

							// 랜덤 엔진 및 분포 생성
							std::random_device rd;
							std::mt19937 gen(rd());
							std::discrete_distribution<> dist(drop_weights.begin(), drop_weights.end());

							int random_index = dist(gen);
							std::string gun_name = gun_names[random_index];
							Object* dropped_gun = FindModelInfo(gun_names[random_index])->GetInstance();

							XMFLOAT3 drop_pos = monster->owner()->world_position_vector();
							drop_pos.y += 0.1f;
							dropped_gun->set_position_vector(drop_pos);
							dropped_gun->set_is_movable(true);

							BoundingBox gun_bb{ {0.f, 0.f, 0.f}, {0.5f, 0.3f, 1.0f} };
							auto box_comp = new BoxColliderComponent(dropped_gun, gun_bb);
							dropped_gun->AddComponent(box_comp);

							// UI
							/*Object* ui_texture = FindModelInfo("Gun_UI")->GetInstance();
							ui_texture->set_local_position({ 0.0f, 0.5f, 0.1f });
							dropped_gun->AddChild(ui_texture);*/

							std::string dropped_name = dropped_gun->name();  // 예: "Dropped_Classic"

							GunComponent* dropped_gun_component = Object::GetComponent<GunComponent>(dropped_gun);
							std::string gun_ui_name = "Gun_UI_" + dropped_name.substr(dropped_name.find('_') + 1); // "Classic", "Sherif" 등

							// 랜덤 강화, 속성
							int upgrade = rand() % 4;
							dropped_gun_component->set_upgrade(upgrade);

							// [2] 속성 타입: 0 = Fire, 1 = Electric, 2 = Poison
							int element_random = rand() % 3;
							ElementType element = static_cast<ElementType>(element_random);
							dropped_gun_component->set_element(element);

							if (upgrade > 0)
							{
								gun_ui_name += "+" + std::to_string(upgrade);
							}

							AddObject(dropped_gun);
							dropped_guns_.push_back(dropped_gun);

							sc_packet_drop_gun dg;
							dg.size = sizeof(sc_packet_drop_gun);
							dg.type = S2C_P_DROP_GUN;
							dg.id = dropped_gun->id();
							dg.gun_type = random_index;
							dg.upgrade_level = upgrade;
							dg.element_type = element_random;
							XMFLOAT4X4 xf;
							XMFLOAT4X4 mat = dropped_gun->transform_matrix();
							XMStoreFloat4x4(&xf, XMLoadFloat4x4(&mat));
							memcpy(dg.matrix, &xf, sizeof(float) * 16);

							std::cout << "드랍 총 id: " << dg.id << std::endl;
							for (const auto& player : users) {
								player.second->do_send(&dg);
							}
							
						}
					}
				}
			}
		}
	}
}

void BaseScene::CheckPlayerHitPyramid(Object* object)
{
	if (stage_clear_num_ != 6) return; // 스테이지 6에서만 체크
	
	auto mesh_collider = Object::GetComponentInChildren<MeshColliderComponent>(object);
	if (!mesh_collider) return;
	
	BoundingOrientedBox player_obb = mesh_collider->GetWorldOBB();
	
	for (auto& pyramid_collider : checking_maps_mesh_collider_list_[6])
	{
		if (!pyramid_collider || !pyramid_collider->mesh()) continue;
	
		auto name = pyramid_collider->mesh()->name();
	
		if (name != "Pyramid_01") continue;
		BoundingOrientedBox pyramid_obb = pyramid_collider->GetWorldOBB();
	
		if (player_obb.Intersects(pyramid_obb))
		{
			// 피라미드 획득 처리
			get_key_num_++;
	
			// 피라미드 제거 (Scene과 충돌 리스트에서 제거)
			Object* pyramid_object = pyramid_collider->owner();
			MeshComponent* mesh_component = Object::GetComponent<MeshComponent>(pyramid_object);
			MeshColliderComponent* mesh_collider_component = Object::GetComponent<MeshColliderComponent>(pyramid_object);
			//mesh_component->set_is_visible(false); // 대신 피라미드 제거 패킷
			//DeleteObject(pyramid_object); // Scene::DeleteObject 호출 포함됨
			auto& mesh_list = checking_maps_mesh_collider_list_[6];
			mesh_list.remove_if([&](MeshColliderComponent* collider) {
				return collider == mesh_collider_component;
				});
	
			break;
		}
	}
}

void BaseScene::CheckSpawnBoxHitPlayers()
{
	if (stage_clear_num_ < 1 || is_activate_spawner_)
		return;

	const auto& spawn_box = spawn_boxs_[stage_clear_num_ - 1];

	// 모든 플레이어 순회
	for (auto& object : ground_check_object_list_)
	{
		if (!object->is_player())
			continue;

		auto player_meshes = Object::GetComponentsInChildren<MeshComponent>(object);
		for (const auto& mesh : player_meshes)
		{
			BoundingBox mesh_animated_aabb;
			const XMFLOAT4X4& mat = mesh->owner()->world_matrix();
			XMMATRIX world_matrix = XMLoadFloat4x4(&mat);
			mesh->GetMesh()->bounds().Transform(mesh_animated_aabb, world_matrix);
			if (spawn_box->animated_box().Intersects(mesh_animated_aabb))
			{
				// 스폰 활성화 처리
				if (stage_clear_num_ == 4)
				{
					//auto& mesh_list = Object::GetComponentsInChildren<SkinnedMeshComponent>(object);
					//for (auto& mesh : mesh_list)
					//{
					//	mesh->set_is_visible(!mesh->IsVisible());
					//}
					//auto controller = Object::GetComponent<FPSControllerComponent>(object);
					//if (controller)
					//	controller->Stop();
					//cut_scene_tracks_[0].Play(this);

					//컷신을 틀도록 패킷 전송
				}
				
				ActivateStageMonsterSpawner(stage_clear_num_ - 1);
				is_activate_spawner_ = true;
				std::cout << "진입완료" << std::endl;
				return; // 한 명이라도 충돌했으면 나가기
			}
		}
	}
}

void BaseScene::CheckPlayerHitChest()
{
	if (!is_prepare_ground_checking_)
	{
		PrepareGroundChecking();
	}
	for (auto& object : ground_check_object_list_)
	{
		if (!(object->is_player() && !object->is_dead()))
			return;

		Session* session = SessionManager::getInstance().GetSessionByPlayerObject(object);
		if (!session)
			return;

		auto player_mesh_collider = Object::GetComponentInChildren<MeshColliderComponent>(object);
		if (!player_mesh_collider)
		{
			session->SetKeyDown('F', false);
			return;
		}

		BoundingOrientedBox player_obb = player_mesh_collider->GetWorldOBB();

		for (size_t i = 0; i < chests_.size(); ++i)
		{
			Object* chest = chests_[i];

			// 트리거 판정용
			auto box_colliders = Object::GetComponents<BoxColliderComponent>(chest);
			if (box_colliders.empty()) continue;

			for (auto& box_collider : box_colliders)
			{
				if (player_obb.Intersects(box_collider->animated_box()))
				{
					auto chest_component = Object::GetComponent<ChestComponent>(chest);
					if (!chest_component)
						continue;

					chest_component->HendleCollision(object, i);

					if (session->IsKeyDown('F'))
					{
						auto scroll_type = chest_component->TakeScroll(i);
						PlayerComponent* player_comp = Object::GetComponent<PlayerComponent>(object);
						if (player_comp && scroll_type != ScrollType::None)
						{
							player_comp->AddScroll(scroll_type);

							if (scroll_type == ScrollType::kWeaponMaster)
							{
								Object* player_gun_frame = object->FindFrame("WeaponR_locator");
								if (player_gun_frame)
								{
									GunComponent* gun = Object::GetComponentInChildren<GunComponent>(player_gun_frame);
									if (gun)
									{
										gun->set_upgrade(4);
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void BaseScene::CheckRayHitEnemy(const XMFLOAT3& ray_origin, const XMFLOAT3& ray_direction, int id)
{
	XMVECTOR origin = XMLoadFloat3(&ray_origin);
	XMVECTOR direction = XMVector3Normalize(XMLoadFloat3(&ray_direction));

	float closest_t = FLT_MAX;
	Object* closest_monster = nullptr;	// 광선에 부딪힌 가장 가까운 몬스터

	//광선 충돌체크 코드
	for (const auto& monster : monster_list_)
	{
		if (monster->IsDead()) continue;
		const auto& object = monster->owner();

		auto box_list = Object::GetComponentsInChildren<BoxColliderComponent>(object);
		if (box_list.empty()) continue;

		for (auto& box_collider : box_list)
		{
			const BoundingOrientedBox& box = box_collider->animated_box();
			float t = 0.f;
			if (box.Intersects(origin, direction, t))
			{
				if (t < closest_t)
				{
					closest_t = t;
					closest_monster = object;
				}
			}
		}
	}

	if (closest_monster)
	{
		//TODO: hit 사운드를 출력하라고 패킷 송신
		//FMODSoundManager::Instance().PlaySound("hit", false, 0.3f);
		Object* player_ = SessionManager::getInstance().get(id)->get_player_object();
		GunComponent* gun = Object::GetComponentInChildren<GunComponent>(player_);
		if (!gun) return;

		MonsterComponent* monster = Object::GetComponent<MonsterComponent>(closest_monster);
		if (!monster || monster->IsDead()) return;

		float damage = gun->damage() * (1 + gun->upgrade() * 0.2);
		// 플레이어 스크롤 효과 적용
		PlayerComponent* player_comp = Object::GetComponent<PlayerComponent>(player_);
		if (player_comp)
		{
			if (gun->element() == ElementType::kFire &&
				player_comp->HasScroll(ScrollType::kFlameMaster))
			{
				damage *= 1.3f;
			}
			else if (gun->element() == ElementType::kPoison &&
				player_comp->HasScroll(ScrollType::kAcidMaster))
			{
				damage *= 1.3f;
			}
			else if (gun->element() == ElementType::kElectric &&
				player_comp->HasScroll(ScrollType::kElectricMaster))
			{
				damage *= 1.3f;
			}
		}
		bool flame_frenzy = false;
		bool acid_frenzy = false;
		bool electric_frenzy = false;
		if (player_comp)
		{
			if (player_comp->HasScroll(ScrollType::kFlameFrenzy))
			{
				flame_frenzy = true;
			}
			if (player_comp->HasScroll(ScrollType::kAcidFrenzy))
			{
				acid_frenzy = true;
			}
			if (player_comp->HasScroll(ScrollType::kElectricFrenzy))
			{
				electric_frenzy = true;
			}
		}

		// 속성 효과
		switch (gun->element())
		{
		case ElementType::kFire:
			monster->ApplyStatusEffect(StatusEffectType::Fire, 3.0f, damage, flame_frenzy, acid_frenzy, electric_frenzy);
			break;
		case ElementType::kPoison:
			monster->ApplyStatusEffect(StatusEffectType::Poison, 3.0f, 0.f, flame_frenzy, acid_frenzy, electric_frenzy);
			break;
		case ElementType::kElectric:
			monster->ApplyStatusEffect(StatusEffectType::Electric, 3.0f, 0.f, flame_frenzy, acid_frenzy, electric_frenzy);
			break;
		}
		monster->HitDamage(damage);

		XMFLOAT3 hit_position = ray_origin + (ray_direction * closest_t);
		XMFLOAT4 particle_color = GunComponent::GetGunElementColor(gun);

		sc_packet_monster_damaged_particle particle_packet{};
		particle_packet.color = particle_color;
		particle_packet.position = hit_position;

		const auto& users = SessionManager::getInstance().getAllSessions();
		for (const auto& player : users) {
			player.second->do_send(&particle_packet);
		}


		if (monster->IsDead())
		{
			// 총기 이름 목록
			std::vector<std::string> gun_names = { "Classic", "Sherif", "Specter", "Vandal", "Odin", "Flamethrower" };

			std::vector<int> drop_weights = { 15, 10, 7, 5, 3, 1 }; // 전체 합 = 41

			// 드랍할지 말지: 41% 확률로 총기 드랍, 나머지 59%는 아무것도 안 떨어짐
			if (rand() % 100 >= 41) return; // 59% 확률로 드랍 안 함

			// 랜덤 엔진 및 분포 생성
			std::discrete_distribution<> dist(drop_weights.begin(), drop_weights.end());

			int random_index = dist(kRandomGenerator);
			std::string gun_name = gun_names[random_index];
			Object* dropped_gun = FindModelInfo(gun_names[random_index])->GetInstance();

			XMFLOAT3 drop_pos = monster->owner()->world_position_vector();
			drop_pos.y += 0.1f;
			dropped_gun->set_position_vector(drop_pos);
			dropped_gun->set_is_movable(true);

			BoundingBox gun_bb{ {0.f, 0.f, 0.f}, {0.5f, 0.3f, 1.0f} };
			auto box_comp = new BoxColliderComponent(dropped_gun, gun_bb);
			dropped_gun->AddComponent(box_comp);

			// UI
			/*Object* ui_texture = FindModelInfo("Gun_UI")->GetInstance();
			ui_texture->set_local_position({ 0.0f, 0.5f, 0.1f });
			dropped_gun->AddChild(ui_texture);*/

			std::string dropped_name = dropped_gun->name();  // 예: "Dropped_Classic"

			GunComponent* dropped_gun_component = Object::GetComponent<GunComponent>(dropped_gun);
			std::string gun_ui_name = "Gun_UI_" + dropped_name.substr(dropped_name.find('_') + 1); // "Classic", "Sherif" 등

			// 랜덤 강화, 속성
			int upgrade = rand() % 4;
			dropped_gun_component->set_upgrade(upgrade);

			// [2] 속성 타입: 0 = Fire, 1 = Electric, 2 = Poison
			int element_random = rand() % 3;
			ElementType element = static_cast<ElementType>(element_random);
			dropped_gun_component->set_element(element);

			if (upgrade > 0)
			{
				gun_ui_name += "+" + std::to_string(upgrade);
			}

			AddObject(dropped_gun);
			dropped_guns_.push_back(dropped_gun);

			sc_packet_drop_gun dg;
			dg.size = sizeof(sc_packet_drop_gun);
			dg.type = S2C_P_DROP_GUN;
			dg.id = dropped_gun->id();
			dg.gun_type = random_index;
			dg.upgrade_level = upgrade;
			dg.element_type = element_random;
			XMFLOAT4X4 xf;
			XMFLOAT4X4 mat = dropped_gun->transform_matrix();
			XMStoreFloat4x4(&xf, XMLoadFloat4x4(&mat));
			memcpy(dg.matrix, &xf, sizeof(float) * 16);

			const auto& users = SessionManager::getInstance().getAllSessions();
			for (const auto& player : users) {
				player.second->do_send(&dg);
			}
		}
	}

}

void BaseScene::CheckObjectHitFlamethrow(Object* object, int id)
{
	Object* player_ = SessionManager::getInstance().get(id)->get_player_object();

	GunComponent* gun = Object::GetComponentInChildren<GunComponent>(player_);
	if (gun->gun_name() != "flamethrower") return;

	auto& bullet_list = gun->fired_bullet_list();

	auto& box_collider_list = Object::GetComponentsInChildren<BoxColliderComponent>(object);
	if (!box_collider_list.size())
		return;

	Object* flame_tip = player_->FindFrame("gun_particle_pivot");
	auto flame_collider = Object::GetComponent<BoxColliderComponent>(flame_tip);
	if (!flame_collider) return;

	for (auto& monster_box : Object::GetComponentsInChildren<BoxColliderComponent>(object))
	{
		if (flame_collider->animated_box().Intersects(monster_box->animated_box()))
		{
			// 데미지 적용
			MonsterComponent* monster = Object::GetComponent<MonsterComponent>(object);
			if (monster && !monster->IsDead())
			{
				XMFLOAT3 hit_position = monster->owner()->world_position_vector();
				XMFLOAT4 particle_color = GunComponent::GetGunElementColor(gun);

				sc_packet_monster_damaged_particle particle_packet{};
				particle_packet.color = particle_color;
				particle_packet.position = hit_position;

				monster->HitDamage(gun->damage() * (1 + gun->upgrade() * 0.2));

				if (monster->IsDead())
				{
					// 총기 이름 목록
					std::vector<std::string> gun_names = { "Classic", "Sherif", "Specter", "Vandal", "Odin", "Flamethrower" };

					std::vector<int> drop_weights = { 15, 10, 7, 5, 3, 1 }; // 전체 합 = 41

					// 드랍할지 말지: 41% 확률로 총기 드랍, 나머지 59%는 아무것도 안 떨어짐
					if (rand() % 100 >= 41) return; // 59% 확률로 드랍 안 함

					// 랜덤 엔진 및 분포 생성
					std::discrete_distribution<> dist(drop_weights.begin(), drop_weights.end());

					int random_index = dist(kRandomGenerator);
					std::string gun_name = gun_names[random_index];
					Object* dropped_gun = FindModelInfo(gun_names[random_index])->GetInstance();

					XMFLOAT3 drop_pos = monster->owner()->world_position_vector();
					drop_pos.y += 0.1f;
					dropped_gun->set_position_vector(drop_pos);
					dropped_gun->set_is_movable(true);

					BoundingBox gun_bb{ {0.f, 0.f, 0.f}, {0.5f, 0.3f, 1.0f} };
					auto box_comp = new BoxColliderComponent(dropped_gun, gun_bb);
					dropped_gun->AddComponent(box_comp);

					// UI
					/*Object* ui_texture = FindModelInfo("Gun_UI")->GetInstance();
					ui_texture->set_local_position({ 0.0f, 0.5f, 0.1f });
					dropped_gun->AddChild(ui_texture);*/

					std::string dropped_name = dropped_gun->name();  // 예: "Dropped_Classic"

					GunComponent* dropped_gun_component = Object::GetComponent<GunComponent>(dropped_gun);
					std::string gun_ui_name = "Gun_UI_" + dropped_name.substr(dropped_name.find('_') + 1); // "Classic", "Sherif" 등

					// 랜덤 강화, 속성
					int upgrade = rand() % 4;
					dropped_gun_component->set_upgrade(upgrade);

					// [2] 속성 타입: 0 = Fire, 1 = Electric, 2 = Poison
					int element_random = rand() % 3;
					ElementType element = static_cast<ElementType>(element_random);
					dropped_gun_component->set_element(element);

					if (upgrade > 0)
					{
						gun_ui_name += "+" + std::to_string(upgrade);
					}

					AddObject(dropped_gun);
					dropped_guns_.push_back(dropped_gun);

					sc_packet_drop_gun dg;
					dg.size = sizeof(sc_packet_drop_gun);
					dg.type = S2C_P_DROP_GUN;
					dg.id = dropped_gun->id();
					dg.gun_type = random_index;
					dg.upgrade_level = upgrade;
					dg.element_type = element_random;
					XMFLOAT4X4 xf;
					XMFLOAT4X4 mat = dropped_gun->transform_matrix();
					XMStoreFloat4x4(&xf, XMLoadFloat4x4(&mat));
					memcpy(dg.matrix, &xf, sizeof(float) * 16);

					const auto& users = SessionManager::getInstance().getAllSessions();
					for (const auto& player : users) {
						player.second->do_send(&dg);
					}
				}
			}
		}
		return;
	}
}

void BaseScene::CheckRazerHitEnemy(RazerComponent* razer_component, MonsterComponent* monster_component)
{
	if (!razer_component || !monster_component ||
		monster_component->IsDead() || !razer_component->is_collision_active() || razer_component->is_collided())
		return;

	XMVECTOR ray_origin = XMLoadFloat3(&razer_component->end_position());
	XMVECTOR ray_direction = XMLoadFloat3(&xmath_util_float3::Normalize(razer_component->start_position() - razer_component->end_position()));

	// 레이저가 몬스터와 충돌했는지 확인
	auto monster_box_list = Object::GetComponentsInChildren<BoxColliderComponent>(monster_component->owner());
	for (auto& monster_box : monster_box_list)
	{		
		float t{};
		if (monster_box->animated_box().Intersects(ray_origin, ray_direction, t))
		{
			monster_component->HitDamage(razer_component->damage());
			razer_component->set_is_collided(true);

			XMFLOAT3 hit_position;
			XMStoreFloat3(&hit_position, ray_origin + (ray_direction * t));

			sc_packet_monster_damaged_particle packet{};
			packet.color = XMFLOAT4{ 1.f, 0.f, 0.f, 1.f }; // 레이저 색상
			packet.position = hit_position;

			const auto& users = SessionManager::getInstance().getAllSessions();
			for (const auto& player : users) {
				player.second->do_send(&packet);
			}

		}
	}
}

std::list<MeshColliderComponent*> BaseScene::checking_maps_mesh_collider_list(int index)
{
	return checking_maps_mesh_collider_list_[index];
}

int BaseScene::stage_clear_num()
{
	return stage_clear_num_;
}
