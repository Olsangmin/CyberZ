#include "Session.h"

void Session::do_recv()
{
	DWORD recv_flag = 0;
	memset(&recv_over.over, 0, sizeof(recv_over.over));
	recv_over.wsabuf.len = BUF_SIZE - prev_remain;
	recv_over.wsabuf.buf = recv_over.send_buf + prev_remain;
	WSARecv(socket, &recv_over.wsabuf, 1, 0, &recv_flag, &recv_over.over, 0);
}

void Session::do_send(void* packet)
{
	OVER_EXP* send_data = new OVER_EXP{ reinterpret_cast<char*>(packet) };
	WSASend(socket, &send_data->wsabuf, 1, 0, 0, &send_data->over, 0);
}

void Session::send_login_info_packet()
{
	SC_LOGIN_INFO_PACKET p;
	p.size = sizeof(p);
	p.type = SC_LOGIN_INFO;
	p.id = id;
	std::cout << "Client[" << id << "] <- " << p.type << "패킷 전송 " << std::endl;
	do_send(&p);
}

void Session::send_add_player_packet(int c_id, DirectX::XMFLOAT3 cid_pos, DirectX::XMFLOAT3 cid_rotation, Player_Character_Type cid_type)
{
	SC_ADD_PLAYER_PACKET p;
	p.size = sizeof(p);
	p.type = SC_ADD_PLAYER;
	p.id = c_id;
	p.position = cid_pos;
	p.rotation = cid_rotation;
	p.c_type = cid_type;
	std::cout << "Cliend[" << id << "] <- P" << c_id << " ADD 패킷 전송" << std::endl;
	std::cout << "[" << cid_pos.x << ", " << cid_pos.y << ", " << cid_pos.z << "]" << std::endl;
	do_send(&p);
}

void Session::send_move_packet(int c_id, DirectX::XMFLOAT3 cid_dir, float cid_yaw, bool is_valid)
{
	SC_MOVE_OBJECT_PACKET p;
	p.size = sizeof(p);
	p.type = SC_MOVE_OBJECT;
	p.id = c_id;
	p.dir = cid_dir;
	p.yaw = cid_yaw;
	p.is_valid = is_valid;
	// std::cout << "Cliend[" << id << "] <- " << "이동 패킷 전송" << std::endl;
	// std::cout << "Cliend[" << id << "] - (" << GetPos().x << ", " << GetPos().z << ") " << std::endl;
	do_send(&p);
}

void Session::send_update_packet(int c_id, DirectX::XMFLOAT3 cid_pos, DirectX::XMFLOAT3 cid_rotation)
{
	SC_UPDATE_PLAYER_PACKET p;
	p.size = sizeof(p);
	p.type = SC_UPDATE_PLAYER;
	p.id = c_id;
	p.position = cid_pos;
	p.rotation = cid_rotation;
	do_send(&p);
}

void Session::send_changeAnimation_packet(int c_id, Player_Animation_ST ch_ani)
{
	SC_CHANGE_ANIMATION_PACKET p;
	p.size = sizeof(p);
	p.type = SC_CHANGE_ANIM;
	p.id = c_id;
	p.ani_st = ch_ani;
	do_send(&p);
}

void Session::send_change_Character_type_packet(int c_id, Player_Character_Type type, std::string _name)
{
	SC_CHANGE_CHARACTER_PACKET p;
	p.size = sizeof(p);
	p.type = SC_CHANGE_CHARACTER;
	p.id = c_id;
	p.name = _name;
	p.c_type = type;
	do_send(&p);
}

void Session::send_add_npc_packet(int n_id, DirectX::XMFLOAT3 nid_pos, DirectX::XMFLOAT3 nid_rotation)
{
	SC_ADD_NPC_PACKET p;
	p.size = sizeof(p);
	p.type = SC_ADD_NPC;
	p.id = n_id;
	p.position = nid_pos;
	p.rotation = nid_rotation;
	do_send(&p);
}

void Session::send_move_npc_packet(int n_id, DirectX::XMFLOAT3 pos)
{
	SC_MOVE_NPC_PACKET p;
	p.size = sizeof(p);
	p.type = SC_MOVE_NPC;
	p.id = n_id;
	p.next_pos = pos;
	do_send(&p);
}

void Session::send_player_death_packet(int p_id)
{
	SC_PLAYER_DEATH_PACKET p;
	p.size = sizeof(p);
	p.type = SC_PLAYER_DEATH;
	p.id = p_id;
	do_send(&p);
}



