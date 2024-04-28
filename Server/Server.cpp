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
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
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

	std::cout << "Server Start" << std::endl;


	// int num_threads = std::thread::hardware_concurrency();
	int num_threads = 1;
	for (int i = 0; i < num_threads; ++i)
		worker_threads.emplace_back(&Server::Worker_thread, this);
	std::thread timer_thread{ &Server::TimerThread, this };

	
	using frame = std::chrono::duration<int32_t, std::ratio<1, MAX_FRAME>>;
	using ms = std::chrono::duration<float, std::milli>;
	std::chrono::time_point<std::chrono::steady_clock> fps_timer{ std::chrono::steady_clock::now() };

	frame fps{}, frame_count{};
	
	while (true) {
		/*if (!gMap.is_InGame()) {
			fps_timer = std::chrono::steady_clock::now();
			continue;
		}*/
	

		fps = duration_cast<frame>(std::chrono::steady_clock::now() - fps_timer);
		
		// 아직 1/60초가 안지났으면 패스
		if (fps.count() < 1) continue;


		// std::cout << frame_count.count() << std::endl;
		/*if (frame_count.count() & 1) {
			gMap.Update();
		}*/

		// gMap.Update();

		

		frame_count = duration_cast<frame>(frame_count + fps);
		if (frame_count.count() >= MAX_FRAME) {
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
				//int packet_size = p[0];
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
		case OP_SEND:
			delete ex_over;
			break;
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
		{
			std::lock_guard<std::mutex> ll{ clients[c_id].o_lock };
			clients[c_id].state = ST_LOBBY;
			// gMap.StartGame();
		}
		std::cout << "Client[" << c_id << "] Login.\n" << std::endl;
		clients[c_id].send_login_info_packet();
		gMap.cl_ids.push_back(c_id);

		// clients[c_id].SetPos(random_pos[uid(rd)]);

		/*for (auto& cl : clients) {
			{
				std::lock_guard<std::mutex> ll(cl.o_lock);
				if (ST_LOBBY != cl.state) continue;
			}
			if (cl.GetId() == c_id) continue;
			cl.send_add_player_packet(c_id, clients[c_id].GetPos(), clients[c_id].GetRotation());
			clients[c_id].send_add_player_packet(cl.GetId(), cl.GetPos(), cl.GetRotation());
		}*/

		for (auto& cl : clients) {
			{
				std::lock_guard<std::mutex> ll(cl.o_lock);
				if (ST_LOBBY != cl.state) continue;
			}
			if (cl.GetId() == c_id) continue;
			cl.send_change_Character_type_packet(c_id, clients[c_id].GetType());
			clients[c_id].send_change_Character_type_packet(cl.GetId(), cl.GetType());
		}
	}
				 break;

	case CS_GAME_START: {
		// GameMap& gmap = GameMap::GetInstance();
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

		std::cout << "방장[" << c_id << "] - " << std::endl;
		gMap.StartGame();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		std::vector<int> players = gMap.cl_ids;

		for (auto& pl: players) { // 012
			for (auto& others : gMap.cl_ids) {
				clients[pl].send_add_player_packet(others, 
					clients[others].GetPos(),clients[others].GetRotation(), clients[others].GetType());
			}
		}

		/*auto& pl = gMap.cl_ids;
		for (auto& cl : clients) {
			
			cl.send_add_player_packet(c_id, clients[c_id].GetPos(), clients[c_id].GetRotation(), clients[c_id].GetType());
			clients[c_id].send_add_player_packet(cl.GetId(), cl.GetPos(), cl.GetRotation(), cl.GetType());
		}*/

		/*CS_GAMESTART_PACKET* p = reinterpret_cast<CS_GAMESTART_PACKET*>(packet);
		TIMER_EVENT ev{ -99, 0, std::chrono::system_clock::now() + std::chrono::seconds(3), EV_SEND_COUNT };
		timer_queue.push(ev);*/
		break;
	}
	case CS_LOGOUT: {
		CS_LOGOUT_PACKET* p = reinterpret_cast<CS_LOGOUT_PACKET*>(packet);
		{
			std::lock_guard<std::mutex> ll{ clients[c_id].o_lock };
			clients[c_id].state = ST_FREE;
			// gMap.EndGame();
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
		
		// std::cout << "Client[" << c_id << "] Move.";

	}
				break;

	case CS_UPDATE_PLAYER: {
		CS_UPDATE_PLAYER_PACKET* p = reinterpret_cast<CS_UPDATE_PLAYER_PACKET*>(packet);
		{
			std::lock_guard<std::mutex> ll{ clients[c_id].o_lock };
			clients[c_id].SetPos(p->position);
			clients[c_id].SetRotation(p->rotate);
		}
	}
						 break;

	case CS_CHANGE_ANIM: {
		CS_CHANGE_ANIMATION_PACKET* p = reinterpret_cast<CS_CHANGE_ANIMATION_PACKET*>(packet);
		for (auto& cl : clients) {
			if (cl.state != ST_INGAME) continue;
			cl.send_changeAnimation_packet(c_id, p->ani_st);
		}
		std::cout << "Client[" << c_id << "] Anim_Change. \n";
	}
					   break;
	case CS_TEST: {
		CS_TEST_PACKET* p = reinterpret_cast<CS_TEST_PACKET*>(packet);
		// gMap.StartGame();
		
	}
				break;

	case CS_CHANGE_CHARACTER: {
		CS_CHANGE_CHARACTER_PACKET* p = reinterpret_cast<CS_CHANGE_CHARACTER_PACKET*>(packet);

		clients[c_id].SetType(p->c_type);

		for (auto& cl : clients) {
			if (cl.state != ST_LOBBY) continue;
			cl.send_change_Character_type_packet(c_id, clients[c_id].GetType());
		}
	}
							break;

	default: {
		std::cout << "정의되지 않은 패킷 - " << packet[1] << "\n" << std::endl;
		break;
	}
	}
}

void Server::Disconnect(int c_id)
{
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
		if (timer_queue.try_pop(ev)) { // 비었거나, 팝성공

			if (ev.wakeup_time > current_time) {
				timer_queue.push(ev);
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				continue;
			}

			// 성공했을때
			switch (ev.ev_type)
			{
			case EV_NPC_MOVE:
				std::cout << "npc move" << std::endl; break;

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
		// 비었을 때의 작업
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}
