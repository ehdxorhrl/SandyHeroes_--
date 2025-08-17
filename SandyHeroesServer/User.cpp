#include "stdafx.h"
#include "Object.h"
#include "GameFramework.h"
#include "User.h"
#include "SessionManager.h"
#include "GunComponent.h"
#include "MovementComponent.h"
#include "Scene.h"
#include "BaseScene.h"
#include "MonsterComponent.h"
#include "PlayerComponent.h"

Session::Session() {
	std::cout << "DEFAULT SESSION CONSTRUCTOR CALLED!!\n";
	exit(-1);
}
Session::Session(long long session_id, SOCKET s)
	: id_(session_id), c_socket_(s)
{
	remained_ = 0;
}
Session::~Session()
{
	sc_packet_leave lp;
	lp.size = sizeof(lp);
	lp.type = S2C_P_LEAVE;
	lp.id = id_;
	const auto& users = SessionManager::getInstance().getAllSessions();
	for (auto& g : users) {
		if (id_ != g.first) {
			g.second->do_send(&lp);
		}
	}
	closesocket(c_socket_);
}

bool Session::IsKeyDown(int key_code) const {
	auto it = is_key_down_.find(key_code);
	return it != is_key_down_.end() && it->second;
}

void Session::SetKeyDown(int key_code, bool is_down) {
	is_key_down_[key_code] = is_down;
}

void Session::update(float elapsed_time)
{
	if (!player_object_)
		return;

	const auto& movement = Object::GetComponent<MovementComponent>(player_object_);
	BaseScene* base_scene = dynamic_cast<BaseScene*>(GameFramework::Instance()->GetScene());

	XMFLOAT3 original_position = player_object_->world_position_vector();

	XMFLOAT3 velocity{ 0,0,0 };
	float speed = 5.5f;
	XMFLOAT3 look = player_object_->look_vector();
	XMFLOAT3 right = player_object_->right_vector();
	look.y = 0.f; // xz 평면을 따라 이동
	right.y = 0.f;
	look = xmath_util_float3::Normalize(look);
	right = xmath_util_float3::Normalize(right);

	if (is_key_down_['W']) movement->MoveXZ(look.x, look.z, speed);
	if (is_key_down_['S']) movement->MoveXZ(-look.x, -look.z, speed);
	if (is_key_down_['A']) movement->MoveXZ(-right.x, -right.z, speed);
	if (is_key_down_['D']) movement->MoveXZ(right.x, right.z, speed);

	if (is_jumpkey_pressed_)
	{
		std::cout << "점프 전 속도 y값: " << movement->velocity().y << std::endl;
		movement->Jump(jump_speed_);
		is_jumpkey_pressed_ = false;

		std::cout << "점프 전 속도 y값: " << movement->velocity().y << std::endl;
	}

	if (base_scene)
	{
		base_scene->CheckPlayerHitWall(player_object_, movement);

		// 벽 충돌로 이동이 멈췄으면 위치 되돌리기
		if (movement->velocity().x == 0 && movement->velocity().z == 0)
		{
			player_object_->set_position_vector(original_position);
		}

		int stage_num = base_scene->stage_clear_num();
		if (stage_num == 3)
		{
			if (player_object_->position_vector().y < -10.f)
			{
				player_object_->set_position_vector(XMFLOAT3(63.6f, 5.f, -40.f));
			}
		}

	}

	// 대쉬
	constexpr float kDashSpeed = 35.f;
	if (is_dash_pressed_)
	{
		is_dash_pressed_ = false;
	
		XMFLOAT3 position = player_object_->world_position_vector();
	
		constexpr float kGroundYOffset = 0.75f;
		position.y += kGroundYOffset;
		XMVECTOR ray_origin = XMLoadFloat3(&position);
		position.y -= kGroundYOffset;
	
		XMVECTOR ray_direction = XMLoadFloat3(&dash_velocity_);
		ray_direction = XMVectorSetY(ray_direction, 0);
		ray_direction = XMVector3Normalize(ray_direction);
	
		if (0 == XMVectorGetX(XMVector3Length(ray_direction)))
			return;
	
		float distance{ std::numeric_limits<float>::max() };
		BaseScene* base_scene = dynamic_cast<BaseScene*>(GameFramework::Instance()->GetScene());
		for (auto& mesh_collider : base_scene->checking_maps_mesh_collider_list(base_scene->stage_clear_num()))
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
	
		if (distance > 0 && distance < dash_length_)
		{
			if (distance < 1.5)
				dash_length_ = 0.0f;
			else
				dash_length_ = distance - 1.5f;
		}
	
		movement->set_max_speed_xz(kDashSpeed);
		movement->MoveXZ(dash_velocity_.x, dash_velocity_.z, kDashSpeed);
	}

	// 연사
	if (is_firekey_down_)
	{
		GunComponent* gun = Object::GetComponentInChildren<GunComponent>(player_object_);
		if (gun && gun->fire_type() == GunFireType::kAuto) {
			BaseScene* base_scene = dynamic_cast<BaseScene*>(GameFramework::Instance()->GetScene());

			auto bullet_mesh = base_scene->FindModelInfo("SM_Bullet_01")->GetInstance();
			gun->FireBullet(fire_direction_, bullet_mesh, base_scene, id_);
			if (gun->gun_name() == "flamethrower")
			{
				for (const auto& monster : base_scene->monster_list())
				{
					base_scene->CheckObjectHitFlamethrow(monster->owner(), id_, elapsed_time);
				}
			}
		}
	}


	if (xmath_util_float3::Length(player_object_->position_vector() - dash_before_position_) >= dash_length_)
	{
		movement->set_max_speed_xz(speed);
		//AnimatorComponent* animator = Object::GetComponent<AnimatorComponent>(player_object_);
		//if (animator)
		//{
		//	animator->animation_state()->ChangeAnimationTrack((int)PlayerAnimationTrack::kIdle, player_object_, animator);
		//}
	}
	dash_cool_delta_time_ -= elapsed_time;

}

void Session::do_recv()
{
	DWORD recv_flag = 0;
	ZeroMemory(&recv_over_.over_, sizeof(recv_over_.over_));
	recv_over_.wsabuf_[0].buf = reinterpret_cast<CHAR*>(recv_over_.buffer_ + remained_);
	recv_over_.wsabuf_[0].len = sizeof(recv_over_.buffer_) - remained_;
	
	auto ret = WSARecv(c_socket_, recv_over_.wsabuf_, 1, NULL,
		&recv_flag, &recv_over_.over_, NULL);
	if (0 != ret) {
		auto err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no) {
			//print_error_message(err_no);
			exit(-1);
		}
	}
}
void Session::do_send(void* buff)
{
	EXP_OVER* o = new EXP_OVER(IO_SEND);
	const unsigned char packet_size = reinterpret_cast<unsigned char*>(buff)[0];
	memcpy(o->buffer_, buff, packet_size);
	o->wsabuf_[0].len = packet_size;
	DWORD size_sent;
	WSASend(c_socket_, o->wsabuf_, 1, &size_sent, 0, &(o->over_), NULL);
}

void Session::send_player_info_packet()
{
	sc_packet_user_info ip;
	ip.size = sizeof(ip);
	ip.type = S2C_P_USER_INFO;
	ip.id = id_;
	ip.position[0] = player_object_->position_vector().x;
	ip.position[1] = player_object_->position_vector().y;
	ip.position[2] = player_object_->position_vector().z;

	do_send(&ip);
}

void Session::send_player_position()
{
	sc_packet_move mp;
	mp.size = sizeof(mp);
	mp.type = S2C_P_MOVE;
	mp.id = id_;
	XMFLOAT4X4 xf;
	XMFLOAT4X4 mat = player_object_->transform_matrix();
	XMStoreFloat4x4(&xf, XMLoadFloat4x4(&mat));
	memcpy(mp.matrix, &xf, sizeof(float) * 16);
	auto player_component = Object::GetComponent<PlayerComponent>(player_object_);
	mp.main_skill_gage = player_component->main_skill_gage();
	float dash_cool_time = dash_cool_time_ - dash_cool_delta_time_;
	if (dash_cool_time < 0)
		dash_cool_time = 0.f;
	if(dash_cool_time > dash_cool_time_)
		dash_cool_time = dash_cool_time_;
	mp.dash_cool_time = dash_cool_time;
	do_send(&mp);
	//std::cout << "x: " << player_object_->position_vector().x << std::endl;
	//std::cout << "y: " << player_object_->position_vector().y << std::endl;
	//std::cout << "z: " << player_object_->position_vector().z << std::endl;
	//std::cout << player_object_->position_vector().y << std::endl;
}

void Session::process_packet(unsigned char* p, float elapsed_time)
{
	const unsigned char packet_type = p[1];
	static int _z = 0;
	switch (packet_type) {
	case C2S_P_LOGIN:
	{
		cs_packet_login* packet = reinterpret_cast<cs_packet_login*>(p);
		std::cout << "[" << id << "]번 클라이언트가 접속했습니다." << std::endl;

		// Scene이 없다면 한 번만 생성
		if (!GameFramework::Instance()->GetScene())
			GameFramework::Instance()->SetScene<BaseScene>();

		BaseScene* scene = dynamic_cast<BaseScene*>(GameFramework::Instance()->GetScene());

		player_object_ = scene->CreateAndRegisterPlayer(id_);
		id_ = id++;	

		send_player_info_packet();

		//GunComponent* gun_component = Object::GetComponent<GunComponent>(gun);
		//sc_packet_gun_change gc;
		//gc.size = sizeof(gc);
		//gc.type = S2C_P_GUN_CHANGE;
		//gc.id = get_id(); // 클라이언트에서 어떤 플레이어에 해당하는지 식별
		//gc.gun_id = gun_component->owner()->id();
		//std::cout << "교체 총 id: " << gun_component->owner()->id() << std::endl;
		//strcpy_s(gc.gun_name, gun_name.c_str());
		//gc.upgrade_level = gun_component->upgrade();
		//gc.element_type = static_cast<int>(gun_component->element());

		sc_packet_enter ep;
		ep.size = sizeof(ep);
		ep.type = S2C_P_ENTER;
		ep.id = id_;
		strcpy_s(ep.name, name_.c_str());
		XMFLOAT4X4 xf;
		XMFLOAT4X4 mat = player_object_->transform_matrix();
		XMStoreFloat4x4(&xf, XMLoadFloat4x4(&mat));
		memcpy(ep.matrix, &xf, sizeof(float) * 16);

		//std::cout << "[DEBUG] AddObject BaseScene addr: " << scene << std::endl;
		const auto& users = SessionManager::getInstance().getAllSessions();
		for (auto& u : users) 
		{
			if (u.first != id_)
				u.second->do_send(&ep);
		}

		for (auto& u : users) 
		{
			if (u.first != id_) 
			{
				sc_packet_enter ep;
				ep.size = sizeof(ep);
				ep.type = S2C_P_ENTER;
				ep.id = u.first;
				strcpy_s(ep.name, u.second->name_.c_str());
				XMFLOAT4X4 u_mat = u.second->player_object_->transform_matrix();
				XMStoreFloat4x4(&xf, XMLoadFloat4x4(&u_mat));
				memcpy(ep.matrix, &xf, sizeof(float) * 16);
				do_send(&ep);
			}
		}
		break;
	}
	case C2S_P_KEYBOARD_INPUT:
	{
		cs_packet_keyboard_input* packet = reinterpret_cast<cs_packet_keyboard_input*>(p);	
		is_key_down_[packet->key] = packet->is_down;

		if (is_key_down_[VK_SPACE]) 
		{
			BaseScene* base_scene = dynamic_cast<BaseScene*>(GameFramework::Instance()->GetScene());
			base_scene->CheckObjectIsGround(player_object_);
			if (player_object_->is_ground())
			{
				is_jumpkey_pressed_ = true;
			}
		}

		if (is_key_down_[VK_SHIFT])
		{
			if (dash_cool_delta_time_ <= 0)
			{
				is_dash_pressed_ = true;
				dash_cool_delta_time_ = dash_cool_time_;
				dash_velocity_ = { 0,0,0 };
				dash_length_ = 5.0f;
				dash_before_position_ = player_object_->position_vector();
				XMFLOAT3 look = player_object_->look_vector();
				XMFLOAT3 right = player_object_->right_vector();
				look.y = 0.f;
				right.y = 0.f;
				if (is_key_down_['W'])
				{
					dash_velocity_ += look;
				}
				if (is_key_down_['S'])
				{
					dash_velocity_ -= look;
				}
				if (is_key_down_['D'])
				{
					dash_velocity_ += right;
				}
				if (is_key_down_['A'])
				{
					dash_velocity_ -= right;
				}
				if (xmath_util_float3::Length(dash_velocity_) == 0)
				{
					dash_velocity_ += look;
				}

				sc_packet_player_change_animation pca;
				pca.size = sizeof(sc_packet_player_change_animation);
				pca.type = S2C_P_PLAYER_CHANGE_ANIMATION;
				pca.id = id_;
				pca.animation_track = 1;
				pca.loop_type = 1;

				std::cout << (int)pca.animation_track << std::endl;

				const auto& users = SessionManager::getInstance().getAllSessions();
				for (auto& u : users) {
					u.second->do_send(&pca);
				}
			}
		}

		if (is_key_down_['E']) 
		{
			auto player_component = Object::GetComponent<PlayerComponent>(player_object_);
			if (player_component)
				if (player_component->ActivateMainSkill())
				{
					sc_packet_play_mainskill pm;
					pm.size = sizeof(pm);
					pm.type = S2C_P_PLAY_MAINSKILL;
					pm.id = id_;
					const auto& users = SessionManager::getInstance().getAllSessions();
					for (auto& u : users) {
						u.second->do_send(&pm);
					}
				}
		}

		if (is_key_down_['R'])
		{
			GunComponent* gun = Object::GetComponentInChildren<GunComponent>(player_object_);
			gun->ReloadBullets();
			sc_packet_play_reload_sound prs;
			prs.size = sizeof(sc_packet_play_reload_sound);
			prs.type = S2C_P_PLAY_RELOAD_SOUND;

			do_send(&prs);
		}

		if (is_key_down_['0'])
		{
			BaseScene* base_scene = dynamic_cast<BaseScene*>(GameFramework::Instance()->GetScene());
			base_scene->add_stage_clear_num();
			base_scene->set_is_activate_spawner(false);
			sc_packet_stage_clear sc;
			sc.size = sizeof(sc_packet_stage_clear);
			sc.stage_num = base_scene->stage_clear_num();
			sc.type = S2C_P_STAGE_CLEAR;
			const auto& users = SessionManager::getInstance().getAllSessions();
			for (auto& u : users) {
				u.second->do_send(&sc);
			}
		}

		break;
	}
	case C2S_P_MOUSE_MOVE: {
			cs_packet_mouse_move* packet = reinterpret_cast<cs_packet_mouse_move*>(p);
			player_object_->Rotate(0,
				static_cast<float>(packet->yaw) * 0.1,
				0.f);

			sc_packet_move mp;
			mp.size = sizeof(mp);
			mp.type = S2C_P_MOVE;
			mp.id = id_;
			XMFLOAT4X4 xf;
			XMFLOAT4X4 mat = player_object_->transform_matrix();
			XMStoreFloat4x4(&xf, XMLoadFloat4x4(&mat));
			memcpy(mp.matrix, &xf, sizeof(float) * 16);
			auto player_component = Object::GetComponent<PlayerComponent>(player_object_);
			mp.main_skill_gage = player_component->main_skill_gage();
			float dash_cool_time = dash_cool_time_ - dash_cool_delta_time_;
			if (dash_cool_time < 0)
				dash_cool_time = 0.f;
			if (dash_cool_time > dash_cool_time_)
				dash_cool_time = dash_cool_time_;
			mp.dash_cool_time = dash_cool_time;

			const auto& users = SessionManager::getInstance().getAllSessions();
			for (auto& u : users) {
				u.second->do_send(&mp);
				//std::cout << "x: " << player_object_->position_vector().x << std::endl;
				//std::cout << "y: " << player_object_->position_vector().y << std::endl;
				//std::cout << "z: " << player_object_->position_vector().z << std::endl;
			}

		break;
	}
	case C2S_P_MOUSE_CLICK: {
		cs_packet_mouse_click* packet = reinterpret_cast<cs_packet_mouse_click*>(p);
		is_firekey_down_ = true;
		fire_direction_ = packet->pick_dir;

		// 피킹 방향으로 총알 생성
		GunComponent* gun = Object::GetComponentInChildren<GunComponent>(player_object_);
		if (gun) {
			BaseScene* base_scene = dynamic_cast<BaseScene*>(GameFramework::Instance()->GetScene());

			auto bullet_mesh = base_scene->FindModelInfo("SM_Bullet_01")->GetInstance();
			gun->FireBullet(fire_direction_, bullet_mesh, base_scene, id_);
		}
		break;
	}
	case C2S_P_MOUSE_UNCLICK:
	{
		is_firekey_down_ = false;
	}
		break;
	default:
		std::cout << "Error Invalid Packet Type\n";
		exit(-1);
	}
}

void Session::set_player_object(Object* obj)
{
	player_object_ = obj;
}

void Session::set_camera_object(Object* obj)
{
	camera_object_ = obj;
}

long long Session::get_id() const
{
	return id_;
}

