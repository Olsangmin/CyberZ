#pragma once
#include "Object.h"
// #include "Protocol.h"
constexpr int BUF_SIZE = 4096;

enum COMP_TYPE { OP_ACCEPT, OP_RECV, OP_SEND, OP_NPC_MOVE, OP_NPC_ATTACK, OP_COOL_DOWN
};
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

enum S_STATE {ST_FREE, ST_ALLOC, ST_LOBBY, ST_INGAME};
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
		memset(password, 0, sizeof(password));
	}
	~Session() {}

	void do_recv();
	void do_send(void* packet);

	int Get_prev_remain() const { return prev_remain; }
	void Set_prev_remain(int n) { prev_remain = n; }
	void SetSocket(const SOCKET s) { socket = s; }
	const SOCKET& GetSocket() { return socket; }

	void send_login_info_packet();
	void send_add_player_packet(int c_id, DirectX::XMFLOAT3 cid_pos, DirectX::XMFLOAT3 cid_rotation, Player_Character_Type cid_type);
	void send_move_packet(int c_id, DirectX::XMFLOAT3 cid_dir, float cid_yaw, bool is_valid);
	void send_update_packet(int c_id, DirectX::XMFLOAT3 cid_pos, DirectX::XMFLOAT3 cid_rotation);
	void send_changeAnimation_packet(int c_id, Player_Animation_ST ch_ani);
	void send_change_Character_type_packet(int c_id, Player_Character_Type type, std::string _name);

	void send_add_npc_packet(int n_id, DirectX::XMFLOAT3 nid_pos, DirectX::XMFLOAT3 nid_rotation);
	void send_move_npc_packet(int n_id, DirectX::XMFLOAT3 pos);

	void send_player_death_packet(int p_id);
	

private:
	OVER_EXP recv_over;
	SOCKET	socket;
	int		prev_remain;

public:
	S_STATE state;
	char name[NAME_SIZE];
	char password[NAME_SIZE];
	Player_Animation_ST anim;
};

