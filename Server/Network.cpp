#include "Network.h"

Network& Network::GetInstance()
{
	static Network instance;
	return instance;
}

Network::Network()
{
	s_socket = 0; h_iocp = 0;
	for (int i = 0; i < MAX_USER; ++i) clients[i].SetId(0);
}

void Network::run()
{
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
		exit(-1);
	s_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
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
	c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	ac_over.comp_type = OP_ACCEPT;
	AcceptEx(s_socket, c_socket, ac_over.send_buf, 0, addr_size + 16, addr_size + 16, 0, &ac_over.over);

	
	int num_threads = std::thread::hardware_concurrency();
	for (int i = 0; i < num_threads; ++i)
		worker_threads.emplace_back(&Network::worker_thread, this);
	for (auto& th : worker_threads)
		th.join();
}

void Network::worker_thread()
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
				disconnect(static_cast<int>(key));
				if (ex_over->comp_type == OP_SEND) delete ex_over;
				continue;
			}
		}
		if ((num_bytes == 0) && ((ex_over->comp_type == OP_RECV) || (ex_over->comp_type == OP_SEND))) {
			disconnect(static_cast<int>(key));
			if (ex_over->comp_type == OP_SEND) delete ex_over;
			continue;
		}

		switch (ex_over->comp_type) {
		case OP_ACCEPT: {
			std::cout << "ACCEPT" << std::endl;
			int c_id = get_new_client_id();
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
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(c_socket), h_iocp, c_id, 0);
				clients[c_id].do_recv();
				c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			}
			ZeroMemory(&ac_over.over, sizeof(ac_over.over));
			int addr_size = sizeof(SOCKADDR_IN);
			AcceptEx(s_socket, c_socket, ac_over.send_buf, 0, addr_size + 16, addr_size + 16, 0, &ac_over.over);
		}
					  break;
		case OP_RECV: {
			std::cout << "RECV" << std::endl;
			int remain_data = num_bytes + clients[key].Get_prev_remain();
			char* p = ex_over->send_buf;
			while (remain_data > 0) {
				int packet_size = static_cast<int>(p[0]);
				if (packet_size <= remain_data) {
					process_packet(static_cast<int>(key), p);
					p = p + packet_size;
					remain_data -= packet_size;
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

void Network::process_packet(int c_id, char* packet)
{
	switch (packet[1])
	{
	case CS_LOGIN: {
		CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
		strcpy_s(clients[c_id].name, p->name);
		{
			std::lock_guard<std::mutex> ll{ clients[c_id].o_lock };
			clients[c_id].state = ST_INGAME;
		}
		std::cout << "Client[" << c_id << "] Login.\n" << std::endl;
		clients[c_id].send_login_info_packet();
	}
				 break;
	case CS_LOGOUT: {
		CS_LOGOUT_PACKET* p = reinterpret_cast<CS_LOGOUT_PACKET*>(packet);
		{
			std::lock_guard<std::mutex> ll{ clients[c_id].o_lock };
			clients[c_id].state = ST_FREE;
		}
		disconnect(c_id);
		std::cout << "Client[" << c_id << "] LogOut.\n" << std::endl;
	}
				  break;
	case CS_TEST: {
		CS_TEST_PACKET* p = reinterpret_cast<CS_TEST_PACKET*>(packet);
		std::cout << p->x << " 수신" << std::endl;
	}
				break;

	default: {
		std::cout << "정의되지 않은 패킷 \n" << std::endl;
		break;
	}
	}
}

void Network::disconnect(int c_id)
{
	closesocket(clients[c_id].GetSocket());
	std::lock_guard<std::mutex> ll(clients[c_id].o_lock);
	clients[c_id].state = ST_FREE;
}

int Network::get_new_client_id()
{
	for (int i = 0; i < MAX_USER; ++i) {
		std::lock_guard <std::mutex> ll{ clients[i].o_lock };
		if (clients[i].state == ST_FREE)
			return i;
	}
	return -1;
}
