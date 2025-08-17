#pragma once
#include "stdafx.h"
//version 0.1   ������ �׻� �ٲ��� ��

// ���� �ּҿ� ��Ʈ�� ���߿� �Է¹޴½����� ����
constexpr short SERVER_PORT = 5000;
constexpr char SERVER_ADDR[] = "127.0.0.1";

// SC�� �������� Ŭ���̾�Ʈ��, CS�� Ŭ�󿡼� ������ ������ ��Ŷ
constexpr char S2C_P_USER_INFO = 1;			// �÷��̾� ���ο��� �����ϴ� ����
constexpr char S2C_P_MOVE = 2;				
constexpr char S2C_P_ROTATE = 3;			// �÷��̾� ���콺 ����
constexpr char S2C_P_ENTER = 4;				// �÷��̾� ������ ��ε�ĳ��Ʈ
constexpr char S2C_P_LEAVE = 5;
constexpr char S2C_P_CREATE_BULLET = 6;
constexpr char S2C_P_LOADED_BULLET = 7;
constexpr char S2C_P_PLAYER_DAMAGED = 8;    // �÷��̾ ���� ������
constexpr char S2C_P_PLAYER_DEATH = 9;		// �÷��̾� ���
constexpr char S2C_P_DROP_GUN = 10;
constexpr char S2C_P_GUN_CHANGE = 11;
constexpr char S2C_P_SCROLL_INFO = 12;
constexpr char S2C_P_TAKE_SCROLL = 13;
constexpr char S2C_P_PLAY_MAINSKILL = 14;

constexpr char S2C_P_MONSTER_INFO = 21;		// ���� ����
constexpr char S2C_P_MONSTER_MOVE = 22;     // ���� ����
constexpr char S2C_P_MONSTER_DAMAGED = 23;  // ���Ͱ� ���� ������
constexpr char S2C_P_MONSTER_DEATH = 24;
constexpr char S2C_P_MONSTER_DAMAGED_PATICLE = 25;
constexpr char S2C_P_MONSTER_CHANGE_ANIMATION = 26;
constexpr char S2C_P_SHOTDRAGON_ATTACK = 27;

constexpr char S2C_P_STAGE_CLEAR = 55;
constexpr char S2C_P_OBJECT_SET_DEAD = 56;
constexpr char S2C_P_PLAY_RELOAD_SOUND = 57;
constexpr char S2C_P_PLAY_CUT_SCENE = 58;

constexpr char C2S_P_LOGIN = 101;
constexpr char C2S_P_KEYBOARD_INPUT = 102;
constexpr char C2S_P_MOUSE_MOVE = 103;
constexpr char C2S_P_MOUSE_CLICK = 104;
constexpr char C2S_P_MOUSE_UNCLICK = 105;

constexpr char MAX_ID_LENGTH = 20;

constexpr char PRESS_OFF = 0;
constexpr char PRESS_ON = 1;

#pragma pack (push,1)

struct sc_packet_play_cut_scene {
    uint8_t size;
    uint8_t type; // S2C_P_PLAY_CUT_SCENE
    uint8_t cut_scene_track; // �ִϸ��̼� Ʈ��
};

struct sc_packet_play_reload_sound {
    uint8_t size;
    uint8_t type; // S2C_P_PLAY_RELOAD_SOUND
};

struct sc_packet_object_set_dead {
    uint8_t size;
    uint8_t type;  // S2C_P_OBJECT_SET_DEAD
    uint32_t id;    // object id
};

struct sc_packet_shotdragon_attack {
    uint8_t size;
    uint8_t type;  // S2C_P_SHOTDRAGON_ATTACK
    uint32_t id;   // ���� id
    uint32_t thorn_id; // ������ ���� thorn id
    float dx, dy, dz;
	XMFLOAT3 position; // �߻� ��ġ
};
struct sc_packet_monster_change_animation {
    uint8_t size;
    uint8_t type;  // S2C_P_MONSTER_CHANGE_ANIMATION
    uint32_t id;   // ���� id
    uint8_t animation_track; // �ִϸ��̼� Ʈ��
    uint8_t loop_type; // �ִϸ��̼� Ƚ��
};

struct sc_packet_play_mainskill {
    uint8_t size;
    uint8_t type;  // S2C_P_PLAY_MAINSKILL
    uint32_t id;
};

struct sc_packet_take_scroll {
    uint8_t size;
    uint8_t type;  // S2C_P_TAKE_SCROLL
    uint8_t chest_num;
};

struct sc_packet_scroll_info {
    uint8_t size;
    uint8_t type;  // S2C_P_SCROLL_INFO
    uint8_t scroll_type;
    uint8_t chest_num;
};


struct sc_packet_gun_change {
    uint8_t size;
    uint8_t type;  // S2C_GUN_CHANGE
    uint32_t id;
    uint32_t gun_id;
    char gun_name[32];
    uint8_t upgrade_level;
    uint8_t element_type;
};


struct sc_packet_drop_gun
{
    uint8_t  size;           // ��Ŷ ��ü ũ��
    uint8_t  type;           // ��Ŷ Ÿ�� (��: S2C_P_DROP_GUN)
    uint32_t id;             // ����� �ѱ��� ���� ID
    uint8_t  gun_type;       // �ѱ� ���� (0=Classic, ..., 5=Flamethrower)
    uint8_t  upgrade_level;  // ��ȭ ��ġ (0~3)
    uint8_t  element_type;   // �Ӽ� (0=Fire, 1=Electric, 2=Poison)
    float    matrix[16];     // ����� �ѱ��� ��ġ/ȸ���� ������ ��ȯ ���
};


struct sc_packet_loaded_bullet {
    uint8_t size;
    uint8_t type;
    uint32_t id;
    uint8_t loaded_bullets;
};


struct sc_packet_monster_move {
    uint8_t size;
    uint8_t type;
    uint32_t id;
    float speed;
    float matrix[16];
    uint8_t animation_track;
};

struct sc_packet_monster_damaged {
    uint8_t size;
    uint8_t type;
    uint32_t id;
    int32_t hp;
    int32_t shield;
};

struct sc_packet_monster_info {
	uint8_t size;
	uint8_t type;
	uint32_t id;            
	float matrix[16];
	int32_t max_hp;         
	int32_t max_shield;     
	int32_t attack_force;
    int32_t monster_type;
    uint8_t animation_track;
};

struct sc_packet_monster_damaged_particle
{
    uint8_t size = sizeof(sc_packet_monster_damaged_particle);
    uint8_t type = S2C_P_MONSTER_DAMAGED_PATICLE;
    XMFLOAT4 color;
    XMFLOAT3 position;
};

struct sc_packet_stage_clear {
    uint8_t size;
    uint8_t type;
    uint8_t stage_num;
};

struct sc_packet_player_damaged {
    uint8_t size;
    uint8_t type;
    uint32_t id;  // ���� id
    uint32_t hp;
    uint32_t shield;
};

struct sc_packet_user_info {
    uint8_t size;
    uint8_t type;
    uint32_t id;
    float position[3];   // x, y, z
    uint8_t stage_num;
};

struct sc_packet_move {
    uint8_t size;
    uint8_t type;
    uint32_t id;
    float matrix[16]; // �״�� ���� (����ȭ ���� ����)
    float main_skill_gage;
	float dash_cool_time; // �뽬 ��Ÿ��
};

struct sc_packet_rotate {
    uint8_t size;
    uint8_t type;
    uint32_t id;
    float matrix[16]; // �״�� ����
};

struct sc_packet_enter {
    uint8_t size;
    uint8_t type;
    uint32_t id;
    char name[16];     // MAX_ID_LENGTH == 16 ����
    float matrix[16];
};

struct sc_packet_leave {
    uint8_t size;
    uint8_t type;
    uint32_t id;
};

struct sc_packet_create_bullet {
    uint8_t size;
    uint8_t type;
    uint32_t id;
    float dx, dy, dz; // dx, dy, dz
    uint8_t loaded_bullets;  // ���� �Ѿ�
};


struct cs_packet_login {
    uint8_t size;
    uint8_t type;
};

struct cs_packet_keyboard_input {
    uint8_t size;
    uint8_t type;
    uint8_t key;
    uint8_t is_down;  // bool�� �÷��� �� ���̷� uint8 ���
};

struct cs_packet_mouse_move {
    uint8_t size;
    uint8_t type;
    float yaw;
};

struct cs_packet_mouse_click {
    uint8_t size;
    uint8_t type;
    XMFLOAT3 pick_dir;
};

struct cs_packet_mouse_unclick {
    uint8_t size = sizeof(cs_packet_mouse_unclick);
    uint8_t type = C2S_P_MOUSE_UNCLICK;
};

#pragma pack (pop)