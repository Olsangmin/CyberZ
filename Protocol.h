#pragma once
#include <DirectXMath.h>
#include <array>

constexpr int PORT_NUM = 9900;
constexpr int MAX_USER = 3;
constexpr int NUM_NPC = 3;

constexpr int MAX_FRAME = 60;

constexpr int NAME_SIZE = 20;
constexpr int PW_SIZE = 20;

using namespace DirectX;

static std::array<DirectX::XMFLOAT3, MAX_USER> PlayerInitPos = {
	XMFLOAT3(700.f, 0.f, 200.f), XMFLOAT3(900.f, 0.f, 400.f),
	XMFLOAT3(400.f, 0.f, 500.f) };

static std::array<DirectX::XMFLOAT3, MAX_USER> PlayerInitPos_Stage2 = {
	XMFLOAT3(300.f, 0.f, 100.f), XMFLOAT3(80.f, 0.f, 440.f),
	XMFLOAT3(270.f, 0.f, 580.f) };

static std::array<DirectX::XMFLOAT3, MAX_USER> NPCInitPos = {
	XMFLOAT3(400.f, 0.f, 650.f), XMFLOAT3(370.f, 0.f, 170.f),
	XMFLOAT3(760.f, 0.f, 950.f) };

static std::array<DirectX::XMFLOAT3, 3> MissionPos = {
	XMFLOAT3(100.f, 0.0f, 930.f), XMFLOAT3(650.f, 0.f, 100.f),
	XMFLOAT3(500.f, 0.f, 500.f) };

static std::array<DirectX::XMFLOAT3, 5> MissionPos_Stage2 = {
	XMFLOAT3(317.f, 0.f, 161.f), XMFLOAT3(84.f, 0.f, 595.f),
	XMFLOAT3(325.f, 0.f, 379.f), XMFLOAT3(140.f, 0.f, 300.f),
	XMFLOAT3(80.f, 0.f, 125.f) };


static std::array<DirectX::XMFLOAT3, 3> KeyBox = {
	XMFLOAT3(540.f, 0.f, 300.f), XMFLOAT3(780.f, 0.f, 750.f),
	XMFLOAT3(140.f, 0.f, 230.f) };

constexpr float AttackRange = 7.5f;


enum NPC_BEHAVIOR { PATROL, CHASE, ATTACK };

enum S2_COM_STATE { TURNOFF, TURNON, UNABLE };

// Packet Key
constexpr char CS_LOGIN = 0;
constexpr char CS_LOGOUT = 1;
constexpr char CS_MOVE = 2;
constexpr char CS_UPDATE_PLAYER = 3;
constexpr char CS_CHANGE_ANIM = 4; // 애니메이션 변경
constexpr char CS_CHANGE_CHARACTER = 5; // 캐릭터 변경
constexpr char CS_ALLPLAYER_READY = 6; // 게임 시작(방장)
constexpr char CS_GAME_START = 7; // 게임 시작(서버)
constexpr char CS_ALIVE_PLAYER = 8;
constexpr char CS_NPC_UPDATE = 9;


constexpr char CS_GETKEY = 21;
constexpr char CS_GO_STAGE2 = 22;

constexpr char CS_CHANGE_COMST = 30;

constexpr char CS_TEST = 200;
constexpr char CS_SIGNUP = 201;
constexpr char CS_ENTER_ROOM = 202;

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
constexpr char SC_PLAYER_DEATH = 10;
constexpr char SC_PLAYER_ALIVE = 11;
constexpr char SC_NPC_UPDATE = 12;


constexpr char SC_GETKEY = 21;
constexpr char SC_GO_STAGE2 = 22;

constexpr char SC_CHANGE_COMST = 30;


constexpr char SC_TEST = 200;
constexpr char SC_MESSAGE = 255;


enum Player_Animation_ST {
	IDLE, WALK, RUN, CREEP, CRAWL,
	JUMP, HIT, DIE, USE, EAT, DUMMY, NONE = 100
};

enum Player_Character_Type {
	Corzim, Evan, Uranya, Robot
};

enum Player_Interaction_Type {
	CardMission, Heal, FinalMission, NON = 100
};

#pragma pack (push, 1)
struct CS_LOGIN_PACKET {
	unsigned char size;
	char	type;
	char	name[NAME_SIZE];
	char	PW[NAME_SIZE];
};

struct CS_SIGNUP_PACKET {
	unsigned char size;
	char	type;
	char	name[NAME_SIZE];
	char	PW[NAME_SIZE];
};

struct CS_ENTER_ROOM_PACKET {
	unsigned char size;
	char	type;
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

struct CS_ALIVE_PLAYER_PACKET {
	unsigned char size;
	char	type;
	int id;
};

struct CS_TEST_PACKET {
	unsigned char size;
	char	type;
	int	x;
};

struct CS_GO_STAGE2_PACKET {
	unsigned char size;
	char	type;
};

struct CS_CHANGE_COMST_PACKET {
	unsigned char size;
	char	type;
	int comNum;
	S2_COM_STATE state;
};

struct CS_NPC_UPDATE_PACKET {
	unsigned char size;
	char	type;
	int n_id;
	DirectX::XMFLOAT3 position;
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
	std::string name;
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

struct SC_GO_STAGE2_PACKET {
	unsigned char size;
	char	type;
};

struct SC_MESSAGE_PACKET {
	unsigned char size;
	char	type;
	int id;
	std::string message;
};

struct SC_PLAYER_DEATH_PACKET {
	unsigned char size;
	char	type;
	int id;
};

struct SC_PLAYER_ALIVE_PACKET {
	unsigned char size;
	char	type;
	int id;
};

struct SC_NPC_UPDATE_PACKET {
	unsigned char size;
	char	type;
	int n_id;
	DirectX::XMFLOAT3 position;
};

struct SC_CHANGE_COMST_PACKET {
	unsigned char size;
	char	type;
	int p_id;
	int comNum;
	S2_COM_STATE state;
};



struct SC_TEST_PACKET {
	unsigned char size;
	char	type;
	float	x;
};

#pragma pack (pop)