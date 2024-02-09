#pragma once
#include "Object.h"
#include "Protocol.h"

constexpr int BUF_SIZE = 4096;

enum COMP_TYPE { OP_ACCEPT, OP_RECV, OP_SEND };
class OVER_EXP {
	// overlapped 확장 클래스
public:
	WSAOVERLAPPED over;
	WSABUF		wsabuf;
	char		send_buf[BUF_SIZE];
	COMP_TYPE	comp_type;

public:
	OVER_EXP()
	{
		wsabuf.len = BUF_SIZE;
		wsabuf.buf = send_buf;
		comp_type = OP_RECV;
		ZeroMemory(&over, sizeof(over));
	}
	OVER_EXP(char* packet)
	{
		wsabuf.len = packet[0];
		wsabuf.buf = send_buf;
		comp_type = OP_SEND;
		ZeroMemory(&over, sizeof(over));
		memcpy(send_buf, packet, packet[0]);
	}
};

enum S_STATE {ST_FREE, ST_ALLOC, ST_INGAME};
class Session : public Object
{
public:
	Session() 
	{ 
		id = -1; 
		socket = 0;
		prev_remain = 0;
		state = ST_FREE;
		memset(name, 0, sizeof(name));
	}
	~Session() {}

	void do_recv();
	void do_send(void* packet);

	int Get_prev_remain() const { return prev_remain; }
	void Set_prev_remain(int n) { prev_remain = n; }
	void SetSocket(const SOCKET s) { socket = s; }
	const SOCKET& GetSocket() { return socket; }

	void send_login_info_packet();
	void send_move_packet(int c_id);

private:
	OVER_EXP recv_over;
	SOCKET	socket;
	int		prev_remain;

public:
	S_STATE state;
	char name[NAME_SIZE];
};

