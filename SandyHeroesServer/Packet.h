#pragma once
#include "stdafx.h"
//version 0.1   버전은 항상 바꿔줄 것

// 서버 주소와 포트는 나중에 입력받는식으로 수정
constexpr short SERVER_PORT = 5000;
constexpr char SERVER_ADDR[] = "127.0.0.1";

// SC는 서버에서 클라이언트로, CS는 클라에서 서버로 보내는 패킷
constexpr char S2C_P_USER_INFO = 1;			// 플레이어 본인에게 전달하는 인포
constexpr char S2C_P_MOVE = 2;				
constexpr char S2C_P_ROTATE = 3;			// 플레이어 마우스 무브
constexpr char S2C_P_ENTER = 4;				// 플레이어 접속을 브로드캐스트
constexpr char S2C_P_LEAVE = 5;
constexpr char S2C_P_CREATE_BULLET = 6;
constexpr char S2C_P_LOADED_BULLET = 7;
constexpr char S2C_P_PLAYER_DAMAGED = 8;    // 플레이어가 입은 데미지
constexpr char S2C_P_PLAYER_DEATH = 9;		// 플레이어 사망
constexpr char S2C_P_DROP_GUN = 10;
constexpr char S2C_P_GUN_CHANGE = 11;
constexpr char S2C_P_SCROLL_INFO = 12;
constexpr char S2C_P_TAKE_SCROLL = 13;
constexpr char S2C_P_PLAY_MAINSKILL = 14;

constexpr char S2C_P_MONSTER_INFO = 21;		// 몬스터 생성
constexpr char S2C_P_MONSTER_MOVE = 22;     // 몬스터 무브
constexpr char S2C_P_MONSTER_DAMAGED = 23;  // 몬스터가 입은 데미지
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
    uint8_t cut_scene_track; // 애니메이션 트랙
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
    uint32_t id;   // 몬스터 id
    uint32_t thorn_id; // 삭제를 위한 thorn id
    float dx, dy, dz;
	XMFLOAT3 position; // 발사 위치
};
struct sc_packet_monster_change_animation {
    uint8_t size;
    uint8_t type;  // S2C_P_MONSTER_CHANGE_ANIMATION
    uint32_t id;   // 몬스터 id
    uint8_t animation_track; // 애니메이션 트랙
    uint8_t loop_type; // 애니메이션 횟수
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
    uint8_t  size;           // 패킷 전체 크기
    uint8_t  type;           // 패킷 타입 (예: S2C_P_DROP_GUN)
    uint32_t id;             // 드랍된 총기의 고유 ID
    uint8_t  gun_type;       // 총기 종류 (0=Classic, ..., 5=Flamethrower)
    uint8_t  upgrade_level;  // 강화 수치 (0~3)
    uint8_t  element_type;   // 속성 (0=Fire, 1=Electric, 2=Poison)
    float    matrix[16];     // 드랍된 총기의 위치/회전을 포함한 변환 행렬
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
    uint32_t id;  // 유저 id
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
    float matrix[16]; // 그대로 유지 (최적화 여지 있음)
    float main_skill_gage;
	float dash_cool_time; // 대쉬 쿨타임
};

struct sc_packet_rotate {
    uint8_t size;
    uint8_t type;
    uint32_t id;
    float matrix[16]; // 그대로 유지
};

struct sc_packet_enter {
    uint8_t size;
    uint8_t type;
    uint32_t id;
    char name[16];     // MAX_ID_LENGTH == 16 가정
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
    uint8_t loaded_bullets;  // 남은 총알
};


struct cs_packet_login {
    uint8_t size;
    uint8_t type;
};

struct cs_packet_keyboard_input {
    uint8_t size;
    uint8_t type;
    uint8_t key;
    uint8_t is_down;  // bool은 플랫폼 간 차이로 uint8 사용
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