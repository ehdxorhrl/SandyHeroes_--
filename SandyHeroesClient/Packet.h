#pragma once
#include "stdafx.h"

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


constexpr char S2C_P_MONSTER_INFO = 11;		// ���� ����
constexpr char S2C_P_MONSTER_MOVE = 12;     // ���� ����
constexpr char S2C_P_MONSTER_DAMAGED = 13;  // ���Ͱ� ���� ������
constexpr char S2C_P_MONSTER_DEATH = 14;

constexpr char S2C_P_STAGE_CLEAR = 55;


constexpr char C2S_P_LOGIN = 101;
constexpr char C2S_P_KEYBOARD_INPUT = 102;
constexpr char C2S_P_MOUSE_MOVE = 103;
constexpr char C2S_P_MOUSE_CLICK = 104;

constexpr char MAX_ID_LENGTH = 20;

constexpr char PRESS_OFF = 0;
constexpr char PRESS_ON = 1;

#pragma pack (push,1)

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
};

struct sc_packet_stage_clear {
    uint8_t size;
    uint8_t type;
    uint8_t stage_num;
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
    float camera_px, camera_py, camera_pz;  //position
    float camera_lx, camera_ly, camera_lz;  //look
};


#pragma pack (pop)