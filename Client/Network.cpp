#include "Network.h"

Network& Network::GetInstance()
{
	static Network instance;
	return instance;
}

void Network::Start()
{
	wcout.imbue(locale("korean"));
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		cout << "WSA START ERROR" << endl;
	}

	socket = WSASocket(AF_INET, SOCK_STREAM, 0, 0, 0, 0);
	if (socket == INVALID_SOCKET) {
		cout << "SOCKET INIT ERROR!" << endl;
	}

	SOCKADDR_IN server_address{};
	ZeroMemory(&server_address, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT_NUM);
	inet_pton(AF_INET, serverIP.c_str(), &(server_address.sin_addr.s_addr));

	int ret = connect(socket, reinterpret_cast<SOCKADDR*>(&server_address), sizeof(server_address));
	if (ret < 0) exit(-1);
	
	unsigned long noblock = 1;
	ioctlsocket(socket, FIONBIO, &noblock);

	CS_LOGIN_PACKET p;
	p.size = sizeof(p);
	p.type = CS_LOGIN;
	string player_name{ "P" };
	strcpy_s(p.name, player_name.c_str());
	send_packet(&p);
}

void Network::Recv_Packet()
{

	char buf[BUF_SIZE] = { 0 };
	WSABUF wsabuf{ BUF_SIZE, buf };
	DWORD recv_byte{ 0 }, recv_flag{ 0 };

	int retval = WSARecv(socket, &wsabuf, 1, &recv_byte, &recv_flag, nullptr, nullptr);
	
	if (recv_byte > 0) {
		process_data(wsabuf.buf, recv_byte);
	}
}
