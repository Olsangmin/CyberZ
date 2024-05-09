#include "NPC.h"
#include "Server.h"
#include "GameMap.h"

NPC::NPC()
{
	id = -2;
	n_state = NPC_FREE;
	current_behavior = ATTACK;
	near_player = -1;
	my_sector = -1;
}

void NPC::WakeUp(int p_id)
{
	near_player = p_id;

	if (is_active) return;
	bool old_state = false;
	if (false == atomic_compare_exchange_strong(&is_active, &old_state, true))
		return;
	std::cout << "NPC[" << id << "] WakeUp  By P[" << near_player << "] " << std::endl;
	//TIMER_EVENT ev{ id, near_player, std::chrono::system_clock::now() + std::chrono::seconds(3), EV_NPC_MOVE };
	
	
	TIMER_EVENT ev{ id, near_player, std::chrono::system_clock::now(), EV_NPC_MOVE };
	
	
	auto& server = Server::GetInstance();
	server.timer_queue.push(ev);
}



void NPC::DoWork()
{
	if (n_state != NPC_INGAME) return;

	switch (current_behavior)
	{
	case PATROL:
		Patrol();
		break;
	case CHASE:
		Chase();
		break;
	case ATTACK:
		Attack();
		break;

	default:
		break;
	}
	
}

void NPC::Move()
{
	if (n_path.empty())
		return;

	SetPos(n_path.front());
	n_path.pop();
	// std::cout << "NPC[" << id << "] " << current_behavior<< " Move " << GetPos().x << ", " << GetPos().z << std::endl;

}

void NPC::Patrol()
{
	// std::cout << "[" << id << "] Patrol" << std::endl;

	if (n_path.empty()) {
		return;
	}
	
	TIMER_EVENT ev{ id, near_player, std::chrono::system_clock::now()+std::chrono::milliseconds(500), EV_NPC_MOVE};
	auto& server = Server::GetInstance();
	server.timer_queue.push(ev);

	for (auto cl : server.gMap.cl_ids) {
		server.clients[cl].send_move_npc_packet(id, n_path.front());
	}

}

void NPC::Chase()
{
	// std::cout << "[" << id << "] Chase [" <<near_player<< "] " << std::endl;
	if (n_path.empty()) {
		return;
	}

	/*if (distance_near < 100.f) {
		std::cout << "적 발견 경로 재탐색" << std::endl;
		std::queue<DirectX::XMFLOAT3> q{};
		n_path = q;
	}*/
	

	TIMER_EVENT ev{ id, near_player, std::chrono::system_clock::now() + std::chrono::milliseconds(500), EV_NPC_MOVE };
	auto& server = Server::GetInstance();
	server.timer_queue.push(ev);
	

	for (auto cl : server.gMap.cl_ids) {
		server.clients[cl].send_move_npc_packet(id, n_path.front());
	}
}

void NPC::Attack()
{
	std::cout << "[" << id << "] Attack시작 [" << near_player << "] " << std::endl;
	TIMER_EVENT ev{ id, near_player, std::chrono::system_clock::now() + std::chrono::milliseconds(500), EV_NPC_ATTACK };
	auto& server = Server::GetInstance();
	server.timer_queue.push(ev);
}




