#pragma once

#include <DirectXMath.h>

constexpr int PORT_NUM = 4000;
constexpr int MAX_USER = 1000;
constexpr int NUM_NPC = 8;

constexpr int NAME_SIZE = 20;

// Packet Key
constexpr char CS_LOGIN = 0;
constexpr char CS_LOGOUT = 1;
constexpr char CS_MOVE = 2;
constexpr char CS_UPDATE_PLAYER = 3;
constexpr char CS_CHANGE_ANIM = 4; // 애니메이션 변경
constexpr char CS_CHANGE_CHARACTER = 5; // 캐릭터 변경

constexpr char CS_TEST = 200;

// =======================
constexpr char SC_LOGIN_INFO = 0;
constexpr char SC_ADD_PLAYER = 1;
constexpr char SC_MOVE_OBJECT = 2;
constexpr char SC_UPDATE_PLAYER = 3;
constexpr char SC_CHANGE_ANIM = 4;
constexpr char SC_CHANGE_CHARACTER = 5;


constexpr char SC_TEST = 200;

enum Player_Animation_ST {
	IDLE, WALK, RUN, CREEP, CRAWL,
	JUMP, HIT, DIE, USE, EAT, DUMMY, NONE = 100
};

enum Player_Character_Type {
	Corzim, Evan, Uranya, Robot
};

#pragma pack (push, 1)
struct CS_LOGIN_PACKET {
	unsigned char size;
	char	type;
	char	name[NAME_SIZE];
};

struct CS_LOGOUT_PACKET {
	unsigned char size;
	char	type;
};

struct CS_MOVE_PACKET {
	unsigned char size;
	char	type;
	DirectX::XMFLOAT3 dir;
	float	yaw;
};

struct CS_UPDATE_PLAYER_PACKET {
	unsigned char size;
	char	type;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3	rotate;
	Player_Animation_ST ani_st;
};

struct CS_CHANGE_ANIMATION_PACKET {
	unsigned char size;
	char	type;
	Player_Animation_ST ani_st;
};

struct CS_CHANGE_CHARACTER_PACKET { // 캐릭터 변경
	unsigned char size;
	char	type;
	Player_Character_Type c_type;
};

struct CS_TEST_PACKET {
	unsigned char size;
	char	type;
	int	x;
};
// ------------------------------------------
struct SC_LOGIN_INFO_PACKET {
	unsigned char size;
	char		type;
	int		id;
};

struct SC_ADD_PLAYER_PACKET {
	unsigned char size;
	char	type;
	int id;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;
	char name[NAME_SIZE];
};

struct SC_MOVE_OBJECT_PACKET {
	unsigned char size;
	char		type;
	int		id;
	DirectX::XMFLOAT3 dir;
	float	yaw;
	bool		is_valid;
	// unsigned int move_time;
};

struct SC_UPDATE_PLAYER_PACKET {
	unsigned char size;
	char		type;
	int		id;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;
	Player_Animation_ST ani_st;
	// unsigned int move_time;
};

struct SC_CHANGE_ANIMATION_PACKET {
	unsigned char size;
	char	type;
	int id;
	Player_Animation_ST ani_st;
};

struct SC_CHANGE_CHARACTER_PACKET {
	unsigned char size;
	char	type;
	int id;
	Player_Character_Type c_type;
};


struct SC_TEST_PACKET {
	unsigned char size;
	char	type;
	float	x;
};

#pragma pack (pop)