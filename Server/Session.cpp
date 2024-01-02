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
