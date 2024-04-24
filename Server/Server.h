#pragma once
#include "Session.h"
#include "NPC.h"
#include "Timer.h"
#include <concurrent_priority_queue.h>
#include "GameMap.h"
#include <random>


class Server
{

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
	

	void TimerThread();

	std::vector<DirectX::XMFLOAT3> random_pos{ (DirectX::XMFLOAT3(200.f,0.f, 200.f))
	,(DirectX::XMFLOAT3(40.f,0.f, 200.f))
		,(DirectX::XMFLOAT3(300.f,0.f, 200.f)) };

	std::random_device rd{};
	std::uniform_int_distribution<int> uid{0, 2};

public:
	SOCKET s_socket;
	SOCKET c_socket;
	HANDLE h_iocp;
	OVER_EXP ac_over;

	std::array<Session, MAX_USER> clients;
	
	std::vector<std::thread> worker_threads;

	concurrency::concurrent_priority_queue<TIMER_EVENT> timer_queue;

	GameMap gMap{ 1000.f, 1000.f, 100, 100};
	
};

