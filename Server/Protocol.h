#pragma once
#include <DirectXMath.h>

constexpr int PORT_NUM = 4000;
constexpr int MAX_USER = 1000;

constexpr int NAME_SIZE = 20;

// Packet Key
constexpr char CS_LOGIN = 0;
constexpr char CS_LOGOUT = 1;
constexpr char CS_MOVE = 2;
constexpr char CS_TEST = 200;

// =======================
constexpr char SC_LOGIN_INFO = 0;
constexpr char SC_MOVE_OBJECT = 1;
constexpr char SC_ADD_PLAYER = 2;
constexpr char SC_TEST = 200;


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
	DirectX::XMFLOAT3 position;
	Pl_Anim pre_ani; // 0 1
	Pl_Anim after_ani; // 0 1
	float blending_time;
	float	yaw;
};

struct CS_TEST_PACKET {
	unsigned char size;
	char	type;
	float	x;
};
// ------------------------------------------
struct SC_LOGIN_INFO_PACKET {
	unsigned char size;
	char		type;
	int		id;
};

enum Pl_Anim { IDLE, WALK };

struct SC_MOVE_OBJECT_PACKET {
	unsigned char size;
	char		type;
	int		id;
	DirectX::XMFLOAT3 position;
	float	yaw;
	Pl_Anim pre_ani; // 0 1
	Pl_Anim after_ani; // 0 1
	float blending_time;
	bool		is_valid;
	// unsigned int move_time;
};

struct SC_ADD_PLAYER_PACKET {
	unsigned char size;
	char	type;
	int id;
	DirectX::XMFLOAT3 position;
	float yaw;
	char name[NAME_SIZE];
};


struct SC_TEST_PACKET {
	unsigned char size;
	char	type;
	float	x;
};

#pragma pack (pop)