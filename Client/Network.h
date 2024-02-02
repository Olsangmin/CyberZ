#pragma once
#include "stdafx.h"
#include "Common.h"
#include "Protocol.h"
constexpr int BUF_SIZE = 4096;

class Network
{
public:
	static Network& GetInstance();
	Network() {};
	~Network() { closesocket(socket); }
	void Start();
	void Recv_Packet();
	void send_packet(void* packet)
	{
		unsigned char* p = reinterpret_cast<unsigned char*>(packet);
		size_t sent = 0;
		send(socket, reinterpret_cast<char*>(p), static_cast<int>(p[0]), sent);
	}
	void ProcessPacket(char* p)
	{
		switch (p[1])
		{
		case SC_LOGIN_INFO:
		{
			SC_LOGIN_INFO_PACKET* packet = reinterpret_cast<SC_LOGIN_INFO_PACKET*>(p);
			my_id = packet->id;
			cout << "My ID is " << my_id << " !" << endl;
		} break;
		case SC_MOVE_OBJECT:
		{
			SC_MOVE_OBJECT_PACKET* packet = reinterpret_cast<SC_MOVE_OBJECT_PACKET*>(p);
			cout << "[" << packet->id << "] Move (" << packet->x << ", " << packet->y << 
				", "<< packet->z << ")\n";

		}
		break;
		default:
			printf("Unknown PACKET type [%d]\n", p[1]);
		}
	}
	void process_data(char* net_buf, size_t io_byte)
	{
		char* ptr = net_buf;
		static size_t in_packet_size = 0;
		static size_t saved_packet_size = 0;
		static char packet_buffer[BUF_SIZE];

		while (0 != io_byte) {
			if (0 == in_packet_size) in_packet_size = ptr[0];
			if (io_byte + saved_packet_size >= in_packet_size) {
				memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
				ProcessPacket(packet_buffer);
				ptr += in_packet_size - saved_packet_size;
				io_byte -= in_packet_size - saved_packet_size;
				in_packet_size = 0;
				saved_packet_size = 0;
			}
			else {
				memcpy(packet_buffer + saved_packet_size, ptr, io_byte);
				saved_packet_size += io_byte;
				io_byte = 0;
			}
		}

	}
public:
	SOCKET socket{};
	string serverIP{ "127.0.0.1" };
	int my_id = -1;
	
};

