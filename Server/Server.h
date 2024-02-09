#pragma once
#include "Session.h"
class Server
{
public:
	SOCKET s_socket;
	SOCKET c_socket;
	HANDLE h_iocp;
	OVER_EXP ac_over;
	std::array<Session, MAX_USER> clients;
	std::vector<std::thread> worker_threads;
public:
	static Server& GetInstance();
	Server();
	~Server()
	{
		closesocket(s_socket);
		WSACleanup();
	}
	void Network();
	void Worker_thread();
	void Process_packet(int c_id, char* packet);
	void Disconnect(int c_id);
	int Get_new_client_id();
};

