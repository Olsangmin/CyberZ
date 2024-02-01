#pragma once
#include "Session.h"
class Network
{
public:
	SOCKET s_socket;
	SOCKET c_socket;
	HANDLE h_iocp;
	OVER_EXP ac_over;
	std::array<Session, MAX_USER> clients;
	std::vector<std::thread> worker_threads;
public:
	static Network& GetInstance();
	Network();
	~Network() 
	{
		closesocket(s_socket);
		WSACleanup();
	}
	void run();
	void worker_thread();
	void process_packet(int c_id, char* packet);
	void disconnect(int c_id);
	int get_new_client_id();
};

