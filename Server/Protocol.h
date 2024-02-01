#pragma once

constexpr int PORT_NUM = 4000;
constexpr int MAX_USER = 1000;

constexpr int NAME_SIZE = 20;

// Packet Key
constexpr char CS_LOGIN = 0;
constexpr char CS_LOGOUT = 1;
constexpr char CS_TEST = 2;

// =======================
constexpr char SC_LOGIN_INFO = 0;
constexpr char SC_TEST = 1;


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
struct SC_TEST_PACKET {
	unsigned char size;
	char	type;
	float	x;
};

#pragma pack (pop)