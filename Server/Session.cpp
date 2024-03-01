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

void Session::send_move_packet(int c_id, DirectX::XMFLOAT3 cid_pos, bool is_valid)
{
	SC_MOVE_OBJECT_PACKET p;
	p.size = sizeof(p);
	p.type = SC_MOVE_OBJECT;
	p.id = c_id;
	p.position = cid_pos;
	p.yaw = yaw;
	p.is_valid = is_valid;
	std::cout << "Cliend[" << id << "] <- " << "이동 패킷 전송" << std::endl;
	do_send(&p);
}

void Session::send_add_player_packet(int c_id, DirectX::XMFLOAT3 cid_pos)
{
	SC_ADD_PLAYER_PACKET p;
	p.size = sizeof(p);
	p.type = SC_ADD_PLAYER;
	p.id = c_id;
	p.position = cid_pos;
	p.yaw = yaw;
	// std::cout << "Cliend[" << id << "] <- " << "이동 패킷 전송" << std::endl;
	do_send(&p);
}
