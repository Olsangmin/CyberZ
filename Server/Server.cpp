#include "Server.h"

Server& Server::GetInstance()
{
	static Server instance;
	return instance;
}

Server::Server()
{
	s_socket = 0; c_socket = 0; h_iocp = 0;
	for (int i = 0; i < MAX_USER; ++i) {
		clients[i].SetId(-1);
	}

	m_DBConnectionPool = new DBConnectionPool();
}

void Server::Network()
{
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
		exit(-1);
	s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == s_socket)
		ErrorDisplay("socket()");
	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_NUM);
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	bind(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(s_socket, SOMAXCONN);
	int addr_size = sizeof(SOCKADDR_IN);

	h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(s_socket), h_iocp, 9999, 0);
	c_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	ac_over.comp_type = OP_ACCEPT;
	AcceptEx(s_socket, c_socket, ac_over.send_buf, 0, addr_size + 16, addr_size + 16, 0, &ac_over.over);


	gMap.initializeMap();

	gMap.printMap();

	//����
	//if (false == m_DBConnectionPool->Connect(1, L"Driver={ODBC Driver 17 for SQL Server};Server=(localdb)\\MSSQLLocalDB;Database=CyberZDB;Trusted_Connection=Yes;"))
	//{
	//	std::cout << "DB Connect ���� !" << std::endl;
	//	exit(-1);
	//}
	//else
	//{
	//	std::cout << "DB ���� Connected" << std::endl;
	//
	//	// Creat Table
	//	// 
	//	// DROP TABLE IF EXISTS[dbo].[User_Account];			
	//	{
	//		/*auto query = L"									\
	//		CREATE TABLE [dbo].[User_Account]					\
	//		(											\
	//			[id] INT NOT NULL PRIMARY KEY IDENTITY, \
	//			[account_id] NVARCHAR(20) NOT NULL UNIQUE, \
	//			[password] NVARCHAR(20) NOT NULL,						\
	//			[createDate] DATETIME NULL				\
	//		);";
	//
	//		DBConnection* dbConn = m_DBConnectionPool->Pop();
	//		if (false == dbConn->Execute(query)) {
	//			return;
	//		}
	//		m_DBConnectionPool->Push(dbConn);*/
	//	}
	//}



	// ������ Read



	std::cout << "Server Start" << std::endl;


	int num_threads = 1;
	// int num_threads = std::thread::hardware_concurrency();
	for (int i = 0; i < num_threads; ++i)
		worker_threads.emplace_back(&Server::Worker_thread, this);
	std::thread timer_thread{ &Server::TimerThread, this };
	// std::thread db_thread{ &Server::DBThread, this };


	using frame = std::chrono::duration<int, std::ratio<1, MAX_FRAME>>;
	using ms = std::chrono::duration<float, std::milli>;
	std::chrono::time_point<std::chrono::steady_clock> fps_timer{ std::chrono::steady_clock::now() };

	frame fps{}, frame_count{};

	while (true) {
		if (!gMap.is_InGame()) {
			fps_timer = std::chrono::steady_clock::now();
			continue;
		}


		fps = duration_cast<frame>(std::chrono::steady_clock::now() - fps_timer);

		// ���� 1/60�ʰ� ���������� �н�

		if (fps.count() < 1) continue;


		if (frame_count.count() == 0) {
			// std::cout << "PLAYER UPDATE" << std::endl;
			SC_UPDATE_PLAYER_PACKET uPackets[MAX_USER];
			for (int i = 0; auto id : gMap.cl_ids) {
				uPackets[i].size = sizeof(SC_UPDATE_PLAYER_PACKET);
				uPackets[i].type = SC_UPDATE_PLAYER;
				uPackets[i].id = clients[id].GetId();
				uPackets[i].position = clients[id].GetPos();
				uPackets[i].rotation = clients[id].GetRotation();
				++i;
			}

			for (int id : gMap.cl_ids) {
				for (int i = 0; i < MAX_USER; ++i) {
					if (clients[i].state != ST_INGAME) continue;
					clients[id].do_send(&(uPackets[i]));
				}
			}
		}


		gMap.Update(frame_count.count());


		frame_count = duration_cast<frame>(frame_count + fps);
		if (frame_count.count() >= MAX_FRAME) {
			// std::cout << num++ << "��" << std::endl;
			frame_count = frame::zero();
		}
		else {
		}
		fps_timer = std::chrono::steady_clock::now();
	}

	timer_thread.join();
	for (auto& th : worker_threads)
		th.join();



}

void Server::Worker_thread()
{
	while (true) {
		DWORD num_bytes{};
		ULONG_PTR key{};
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(h_iocp, &num_bytes, &key, &over, INFINITE);
		OVER_EXP* ex_over = reinterpret_cast<OVER_EXP*>(over);
		if (false == ret) {
			if (ex_over->comp_type == OP_ACCEPT) std::cout << "Accept Error";
			else {
				std::cout << "GQCS Error on client[" << key << "]\n";
				Disconnect(static_cast<int>(key));
				if (ex_over->comp_type == OP_SEND) delete ex_over;
				continue;
			}
		}
		if ((num_bytes == 0) && ((ex_over->comp_type == OP_RECV) || (ex_over->comp_type == OP_SEND))) {
			Disconnect(static_cast<int>(key));
			if (ex_over->comp_type == OP_SEND) delete ex_over;
			continue;
		}

		switch (ex_over->comp_type) {
		case OP_ACCEPT: {

			int c_id = Get_new_client_id();
			if (c_id == -1)
				std::cout << "Max User exceeded.\n";
			else {
				{
					std::lock_guard<std::mutex> ll(clients[c_id].o_lock);
					clients[c_id].state = ST_ALLOC;
				}
				clients[c_id].SetId(c_id);
				clients[c_id].SetSocket(c_socket);
				clients[c_id].Set_prev_remain(0);
				clients[c_id].SetPos(DirectX::XMFLOAT3(0.f, 0.f, 0.f));
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(c_socket), h_iocp, c_id, 0);
				clients[c_id].do_recv();
				c_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
			}
			ZeroMemory(&ac_over.over, sizeof(ac_over.over));
			int addr_size = sizeof(SOCKADDR_IN);
			AcceptEx(s_socket, c_socket, ac_over.send_buf, 0, addr_size + 16, addr_size + 16, 0, &ac_over.over);
			break;
		}

		case OP_RECV: {
			int remain_data = num_bytes + clients[key].Get_prev_remain();
			char* p = ex_over->send_buf;
			while (remain_data > 0) {
				int packet_size = static_cast<int>(p[0]);
				if (packet_size <= remain_data) {
					Process_packet(static_cast<int>(key), p);
					p = p + packet_size;
					remain_data = remain_data - packet_size;
				}
				else break;
			}
			clients[key].Set_prev_remain(remain_data);
			if (remain_data > 0)
				memcpy(ex_over->send_buf, p, remain_data);
			clients[key].do_recv();
		}
					break;
		case OP_SEND: {
			delete ex_over;
			break;
		}

		case OP_NPC_MOVE: {
			auto& npc = gMap.npcs[key - 100];
			npc.Move();

			delete ex_over;
		}
						break;

		case OP_NPC_ATTACK: {
			auto& npc = gMap.npcs[key - 100];
			if (npc.current_behavior == ATTACK) {
				std::cout << npc.near_player << "���" << std::endl;
				clients[npc.near_player].anim = CRAWL;
				for (int id : gMap.cl_ids) {
					clients[id].send_player_death_packet(npc.near_player);
				}
			}
			npc.IsAttack = false;
			delete ex_over;
		}break;

		default:
			break;
		}


	}
}

void Server::Process_packet(int c_id, char* packet)
{
	switch (packet[1])
	{
	case CS_LOGIN: {
		CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
		strcpy_s(clients[c_id].name, p->name);
		strcpy_s(clients[c_id].password, p->PW);
		{
			std::lock_guard<std::mutex> ll{ clients[c_id].o_lock };
			{
				//if (false == TryLogin(clients[c_id].name, clients[c_id].password))
				//{
				//	return;
				//}
				clients[c_id].state = ST_LOBBY;
			}
		}
		std::cout << "Client[" << c_id << "] Login.\n" << std::endl;
		clients[c_id].send_login_info_packet();

		if (gMap.cl_ids.size() <= 3)
			gMap.cl_ids.push_back(c_id);

		for (auto& cl : clients) {
			{
				std::lock_guard<std::mutex> ll(cl.o_lock);
				if (ST_LOBBY != cl.state) continue;
			}
			if (cl.GetId() == c_id) continue;
			cl.send_change_Character_type_packet(c_id, clients[c_id].GetType());
			clients[c_id].send_change_Character_type_packet(cl.GetId(), cl.GetType());
		}
	} break;
	case CS_SIGNUP: {
		CS_SIGNUP_PACKET* p = reinterpret_cast<CS_SIGNUP_PACKET*>(packet);
		

		DBConnection* dbConn = m_DBConnectionPool->Pop();
		dbConn->Unbind();

		WCHAR name[20] = {};
		MultiByteToWideChar(CP_ACP, 0, p->name, -1, name, 20);
		SQLLEN nameLen = 0;

		if (dbConn->BindParam(1, name, &nameLen))
		{

		}

		WCHAR password[20] = {};
		MultiByteToWideChar(CP_ACP, 0, p->PW, -1, password, 20);
		SQLLEN passwordLen = 0;

		if (dbConn->BindParam(2, password, &passwordLen))
		{

		}

		TIMESTAMP_STRUCT ts = {2024, 07, 01};
		SQLLEN tsLen = 0;

		if (dbConn->BindParam(3, &ts, &tsLen))
		{

		}


		if (dbConn->Execute(L"INSERT INTO [dbo].[User_Account]([account_id], [password], [createDate]) VALUES(?, ?, ?)"))
		{

		}
		m_DBConnectionPool->Push(dbConn);

	}break;

	case CS_ALLPLAYER_READY: {
		if (c_id != gMap.cl_ids[0]) break;
		// GameMap& gmap = GameMap::GetInstance();

		std::sort(gMap.cl_ids.begin(), gMap.cl_ids.end());
		auto u = std::unique(gMap.cl_ids.begin(), gMap.cl_ids.end());
		gMap.cl_ids.erase(u, gMap.cl_ids.end());

		std::set<int> types;
		for (auto id : gMap.cl_ids)
			types.insert(clients[id].GetType());

		if (types.size() != gMap.cl_ids.size()) {
			std::cout << "ĳ���� �ߺ� " << std::endl;
			break;
		}


		for (auto& cl : clients) {
			{
				std::lock_guard<std::mutex> ll(cl.o_lock);
				if (ST_LOBBY != cl.state) continue;
				else {
					// gMap.cl_ids.push_back(cl.GetId());
					cl.state = ST_INGAME;
				}
			}
			// if (cl.GetId() == c_id) continue;
		}

		std::cout << "����[" << c_id << "] - " << std::endl;
		gMap.StartGame();
		break;
	}

	case CS_GAME_START: {
		if (c_id != gMap.cl_ids[0]) break;

		std::vector<int> players = gMap.cl_ids;
		if (gMap.GetStage() == NOGAME) {
			auto& npcs = gMap.npcs;
			for (auto& pl : players) { // 012
				for (auto& others : gMap.cl_ids) {
					clients[pl].send_add_player_packet(others,
						clients[others].GetPos(), clients[others].GetRotation(), clients[others].GetType());
				}

				for (auto& npc : npcs) {
					clients[pl].send_add_npc_packet(npc.GetId(), npc.GetPos(), npc.GetRotation());
					std::cout << pl << " to " << npc.GetId() << std::endl;
				}
			}
			gMap.PlayGame();
		}

		else if (gMap.GetStage() == LOADING) {
			for (auto& pl : players) {
				clients[pl].SetPos(PlayerInitPos_Stage2[clients[pl].GetType()]);
			}
			for (auto& pl : players) {

				for (auto& others : gMap.cl_ids) {
					clients[pl].send_add_player_packet(others,
						clients[others].GetPos(), clients[others].GetRotation(), clients[others].GetType());
				}
			}
			gMap.ChangeToMap2();
			gMap.SetStage(STAGE2);
		}

		break;
	}
	case CS_LOGOUT: {
		CS_LOGOUT_PACKET* p = reinterpret_cast<CS_LOGOUT_PACKET*>(packet);
		{
			std::lock_guard<std::mutex> ll{ clients[c_id].o_lock };
			clients[c_id].state = ST_FREE;
		}
		Disconnect(c_id);
		std::cout << "Client[" << c_id << "] LogOut.\n" << std::endl;
	}
				  break;
	case CS_MOVE: {
		CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);
		DirectX::XMFLOAT3 dir = { p->dir };
		float yaw = p->yaw;

		for (auto& cl : clients) {
			if (cl.state != ST_INGAME) continue;
			cl.send_move_packet(c_id, dir, yaw, true);
		}
	}
				break;

	case CS_UPDATE_PLAYER: {
		CS_UPDATE_PLAYER_PACKET* p = reinterpret_cast<CS_UPDATE_PLAYER_PACKET*>(packet);
		if (gMap.GetStage() == NOGAME || gMap.GetStage() == LOADING)
			break;
		{
			std::lock_guard<std::mutex> ll{ clients[c_id].o_lock };
			clients[c_id].SetPos(p->position);
			clients[c_id].SetRotation(p->rotate);
		}

	}
						 break;

	case CS_CHANGE_ANIM: {
		CS_CHANGE_ANIMATION_PACKET* p = reinterpret_cast<CS_CHANGE_ANIMATION_PACKET*>(packet);
		clients[c_id].anim = p->ani_st;
		for (auto& cl : clients) {
			if (cl.state != ST_INGAME) continue;
			if (cl.GetId() == c_id) continue;
			cl.send_changeAnimation_packet(c_id, p->ani_st);
		}
		// std::cout << "Client[" << c_id << "] Anim_Change. \n";
	}
					   break;

	case CS_CHANGE_CHARACTER: {
		CS_CHANGE_CHARACTER_PACKET* p = reinterpret_cast<CS_CHANGE_CHARACTER_PACKET*>(packet);

		/*for (auto id : gMap.cl_ids) {
			if (id == c_id) continue;
			if (clients[id].GetType() == p->c_type)
				return;
		}*/

		clients[c_id].SetType(p->c_type);

		for (auto& cl : clients) {
			if (cl.state != ST_LOBBY) continue;
			cl.send_change_Character_type_packet(c_id, clients[c_id].GetType());
		}
	}
							break;

	case CS_GETKEY: {
		// CS_GETKEY_PACKET* p = reinterpret_cast<CS_GETKEY_PACKET*>(packet);
		std::cout << c_id << "Űȹ��" << std::endl;
		SC_GETKEY_PACKET keyPacket;
		keyPacket.size = sizeof(keyPacket);
		keyPacket.type = SC_GETKEY;
		keyPacket.p_id = c_id;

		for (auto id : gMap.cl_ids) {
			if (id == c_id) continue;
			clients[id].do_send(&keyPacket);
		}

	}break;

	case CS_ALIVE_PLAYER: {
		CS_ALIVE_PLAYER_PACKET* p = reinterpret_cast<CS_ALIVE_PLAYER_PACKET*>(packet);
		clients[c_id].anim = IDLE;
		SC_PLAYER_ALIVE_PACKET alivePacket;
		alivePacket.size = sizeof(alivePacket);
		alivePacket.type = SC_PLAYER_ALIVE;
		alivePacket.id = c_id;

		for (auto id : gMap.cl_ids) {
			clients[id].do_send(&alivePacket);
		}

	}break;

	case CS_GO_STAGE2: {
		CS_GO_STAGE2_PACKET* p = reinterpret_cast<CS_GO_STAGE2_PACKET*>(packet);
		if (c_id != gMap.cl_ids[0]) break;

		for (int id : gMap.cl_ids) {
			if (clients[id].state != ST_INGAME) continue;
			SC_GO_STAGE2_PACKET packet;
			packet.size = sizeof(packet);
			packet.type = SC_GO_STAGE2;
			clients[id].do_send(&packet);
		}

		gMap.SetStage(GAME_STATE::LOADING);



	}break;

	case CS_TEST: {
		CS_TEST_PACKET* p = reinterpret_cast<CS_TEST_PACKET*>(packet);

	} break;

	default: {
		std::cout << "���ǵ��� ���� ��Ŷ - " << packet[1] << "\n" << std::endl;
		break;
	}
	}
}

void Server::Disconnect(int c_id)
{
	gMap.cl_ids.erase(remove(gMap.cl_ids.begin(), gMap.cl_ids.end(), c_id), gMap.cl_ids.end());

	closesocket(clients[c_id].GetSocket());
	std::lock_guard<std::mutex> ll(clients[c_id].o_lock);
	clients[c_id].state = ST_FREE;
}

int Server::Get_new_client_id()
{
	for (int i = 0; i < MAX_USER; ++i) {
		std::lock_guard <std::mutex> ll{ clients[i].o_lock };
		if (clients[i].state == ST_FREE)
			return i;
	}
	return -1;
}



void Server::TimerThread()
{
	while (true) {
		TIMER_EVENT ev;
		auto current_time = std::chrono::system_clock::now();
		if (timer_queue.try_pop(ev)) { // ����ų�, �˼���

			if (ev.wakeup_time > current_time) {
				timer_queue.push(ev);
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				continue;
			}

			// ����������
			switch (ev.ev_type)
			{
			case EV_NPC_MOVE: {
				OVER_EXP* overE = new OVER_EXP;
				overE->comp_type = OP_NPC_MOVE;
				PostQueuedCompletionStatus(h_iocp, 1, ev.npc_id, &overE->over);

				break;
			}
			case EV_NPC_ATTACK: {
				OVER_EXP* overE = new OVER_EXP;
				overE->comp_type = OP_NPC_ATTACK;
				PostQueuedCompletionStatus(h_iocp, 1, ev.npc_id, &overE->over);
				break;
			}
			case EV_SEND_COUNT: {
				//OVER_EXP* overE = new OVER_EXP;
				//// overE->comp_type
				//PostQueuedCompletionStatus(h_iocp, 1, ev.pl_id, &overE->over);
			}


			default:
				break;
			}
			continue;
		}
		// ����� ���� �۾�
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void Server::DBThread()
{
	using namespace std::chrono;
	while (true)
	{
		
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

bool Server::TryLogin(char* cl_name, char* cl_password)
{
	DBConnection* dbConn = m_DBConnectionPool->Pop();
	dbConn->Unbind();

	
	WCHAR name[20] = {};
	MultiByteToWideChar(CP_ACP, 0, cl_name, -1, name, 20);
	SQLLEN nameLen = 0;
	if (dbConn->BindParam(1, name, &nameLen))
	{

	}


	int outId = 0;
	SQLLEN outIdlen = 0;
	dbConn->BindCol(1, &outId, &outIdlen);

	WCHAR outName[20] = {};
	SQLLEN outNameLen = 0;
	dbConn->BindCol(2, outName, static_cast<int>(sizeof(outName) / sizeof(outName[0])), &outNameLen);

	
	WCHAR outpassword[20] = {};
	SQLLEN outpasswordLen = 0;
	dbConn->BindCol(3, outpassword, static_cast<int>(sizeof(outpassword) / sizeof(outpassword[0])), &outpasswordLen);


	if (dbConn->Execute(L"SELECT id, account_id, password FROM [dbo].[User_Account] WHERE account_id = (?)"))
	{

	}

	bool result = false;
	if (dbConn->Fetch())
	{
		// TODO
		WCHAR clpassword[20] = {};
		MultiByteToWideChar(CP_ACP, 0, cl_password, -1, clpassword, 20);
		if (lstrcmpW(clpassword, outpassword) == 0)
		{
			std::cout << "�α��� ����" << std::endl;
			result = true;
		}
		else
		{
			std::cout << "��й�ȣ�� �ٸ��ϴ�." << std::endl;
			result = false;
		}
	}
	else
	{
		std::cout << "���� ���̵�" << std::endl;
		result = false;
	}

	m_DBConnectionPool->Push(dbConn);

	return result;
	
}
