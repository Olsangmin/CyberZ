#pragma once
#include "Session.h"
#include "NPC.h"
#include "Timer.h"
#include <concurrent_priority_queue.h>
#include "GameMap.h"
#include <random>
#include "DBConnectionPool.h"


class Server
{

public:
	static Server& GetInstance();
	Server();
	~Server()
	{
		closesocket(s_socket);
		WSACleanup();
		delete m_DBConnectionPool;
	}
	void Network();
	void Worker_thread();
	void Process_packet(int c_id, char* packet);
	void Disconnect(int c_id);
	int Get_new_client_id();


	void TimerThread();
	void DBThread();

	bool TryLogin(char* cl_name, char* cl_password);

	/*std::vector<DirectX::XMFLOAT3> random_pos{ (DirectX::XMFLOAT3(500.f,0.f, 500.f))
	,(DirectX::XMFLOAT3(950.f,0.f, 750.f))
		,(DirectX::XMFLOAT3(523.f,0.f, 881.f)) };*/

	std::vector<DirectX::XMFLOAT3> random_pos{ (DirectX::XMFLOAT3(700.f,0.f, 30.f))
	,(DirectX::XMFLOAT3(950.f,0.f, 250.f))
		,(DirectX::XMFLOAT3(100.f,0.f, 881.f)) };

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

	
	DBConnectionPool* m_DBConnectionPool = nullptr;
};

