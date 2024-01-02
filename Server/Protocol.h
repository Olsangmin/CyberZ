#pragma once

constexpr int PORT_NUM = 4000;
constexpr int MAX_USER = 1000;

constexpr int NAME_SIZE = 20;

// Packet Key
constexpr char CS_LOGIN = 0;
constexpr char CS_LOGOUT = 1;

constexpr char SC_LOGIN_INFO = 0;

// =======================


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
// ------------------------------------------
struct SC_LOGIN_INFO_PACKET {
	unsigned char size;
	char		type;
	int		id;
};
#pragma pack (pop)