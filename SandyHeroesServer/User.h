#pragma once
#include "Object.h"
#include "Scene.h"
#include "BaseScene.h"

static int id = 0;

class EXP_OVER
{
public:
	EXP_OVER(IO_OP op) : io_op_(op)
	{
		ZeroMemory(&over_, sizeof(over_));

		wsabuf_[0].buf = reinterpret_cast<CHAR*>(buffer_);
		wsabuf_[0].len = sizeof(buffer_);
	}

	WSAOVERLAPPED	over_;
	IO_OP			io_op_;
	SOCKET			accept_socket_;
	unsigned char	buffer_[1024];
	WSABUF			wsabuf_[1];
};

class Session {
private:
	SOCKET			c_socket_;
	long long		id_;
	std::string		name_;

	EXP_OVER		recv_over_{ IO_RECV };
	Object* player_object_ = nullptr;
	Object* camera_object_ = nullptr;

	std::unordered_map<int, bool> is_key_down_;


private:
	bool is_firekey_down_ = false;

	// ����Ű�� ���Ȱ� owner�� �������� �ִٸ� ������ ����
	bool is_jumpkey_pressed_ = false;
	float jump_speed_ = 6.f;

	float y_axis_velocity_ = 0.f;       // y�࿡ ���� �ӵ�
	float gravity_ = 20.f;

	bool is_dash_pressed_ = false;
	float dash_cool_delta_time_ = 0.f;          // �뽬 ��Ÿ�� ������ ���� ����
	float dash_cool_time_ = 5.f;                // �뽬 ��Ÿ��
	XMFLOAT3 dash_velocity_{ 0,0,0 };           // �뽬 �ӵ�(���� ����Ǵ� "�ӵ�"��)
	float dash_length_{ 10.f };                  // �뽬 �Ÿ�
	XMFLOAT3 dash_before_position_{ 0,0,0 };    // �뽬 ������ġ

public:
	Session();
	Session(long long session_id, SOCKET s);
	~Session();

	void update(float elapsed_time);
	void do_recv();
	void do_send(void* buff);
	void send_player_info_packet();
	void send_player_position();
	void process_packet(unsigned char* p, float elapsed_time);


	void set_player_object(Object* obj);
	void set_camera_object(Object* obj);

	long long get_id() const;

	Object* get_player_object() const { return player_object_; }
	Object* get_camera_object() const { return camera_object_; }

	bool IsKeyDown(int key_code) const;
	void SetKeyDown(int key_code, bool is_down);

public:
	unsigned char	remained_;
};