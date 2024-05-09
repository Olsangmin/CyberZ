#pragma once
#include <DirectXMath.h>
#include <array>

constexpr int PORT_NUM = 9900;
constexpr int MAX_USER = 3;
constexpr int NUM_NPC = 3;

constexpr int MAX_FRAME = 60;

constexpr int NAME_SIZE = 20;

using namespace DirectX;

static std::array<DirectX::XMFLOAT3, MAX_USER> PlayerInitPos = {
	XMFLOAT3(700.f, 0.f, 200.f), XMFLOAT3(900.f, 0.f, 400.f),
	XMFLOAT3(200.f, 0.f, 800.f) };

static std::array<DirectX::XMFLOAT3, MAX_USER> NPCInitPos = {
	XMFLOAT3(450.f, 0.f, 650.f), XMFLOAT3(410.f, 0.f, 220.f),
	XMFLOAT3(750.f, 0.f, 900.f) };

static std::array<DirectX::XMFLOAT3, 3> MissionPos = {
	XMFLOAT3(500.f, 0.f, 500.f), XMFLOAT3(150.f, 0.f, 850.f),
	XMFLOAT3(850.f, 0.f, 150.f) };


static std::array<DirectX::XMFLOAT3, 3> KeyBox = {
	XMFLOAT3(450.f, 0.f, 650.f), XMFLOAT3(150.f, 0.f, 850.f),
	XMFLOAT3(850.f, 0.f, 150.f) };




enum NPC_BEHAVIOR { PATROL, CHASE, ATTACK };

// Packet Key
constexpr char CS_LOGIN = 0;
constexpr char CS_LOGOUT = 1;
constexpr char CS_MOVE = 2;
constexpr char CS_UPDATE_PLAYER = 3;
constexpr char CS_CHANGE_ANIM = 4; // 애니메이션 변경
constexpr char CS_CHANGE_CHARACTER = 5; // 캐릭터 변경
constexpr char CS_ALLPLAYER_READY = 6; // 게임 시작(방장)
constexpr char CS_GAME_START = 7; // 게임 시작(서버)

constexpr char CS_GETKEY = 11;

constexpr char CS_TEST = 200;

// =======================
constexpr char SC_LOGIN_INFO = 0;
constexpr char SC_ADD_PLAYER = 1;
constexpr char SC_MOVE_OBJECT = 2;
constexpr char SC_UPDATE_PLAYER = 3;
constexpr char SC_CHANGE_ANIM = 4;
constexpr char SC_CHANGE_CHARACTER = 5;
constexpr char SC_GAME_START = 6;
constexpr char SC_ADD_NPC = 7;
constexpr char SC_MOVE_NPC = 8;
constexpr char SC_ATTACK_NPC = 9;


constexpr char SC_GETKEY = 11;


constexpr char SC_TEST = 200;
constexpr char SC_MESSAGE = 255;

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

struct CS_GAMESTART_PACKET {
	unsigned char size;
	char	type;
};

struct CS_ALLPLAYER_READY_PACKET {
	unsigned char size;
	char	type;
};

struct CS_GETKEY_PACKET {
	unsigned char size;
	char	type;
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
	Player_Character_Type c_type;
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

struct SC_GAME_START_PACKET {
	unsigned char size;
	char type;
};

struct SC_ADD_NPC_PACKET {
	unsigned char size;
	char	type;
	int id;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;
};

struct SC_MOVE_NPC_PACKET {
	unsigned char size;
	char	type;
	int id;
	DirectX::XMFLOAT3 next_pos;
};

struct SC_ATTACK_NPC_PACKET {
	unsigned char size;
	char	type;
	int n_id;
	int p_id;
};

struct SC_GETKEY_PACKET {
	unsigned char size;
	char	type;
	int p_id;
};

struct SC_MESSAGE_PACKET {
	unsigned char size;
	char	type;
	int id;
	std::string message;
};

struct SC_TEST_PACKET {
	unsigned char size;
	char	type;
	float	x;
};

#pragma pack (pop)