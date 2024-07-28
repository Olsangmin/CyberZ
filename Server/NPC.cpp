#include "NPC.h"
#include "Server.h"
#include "GameMap.h"

NPC::NPC()
{
	id = -2;
	n_state = NPC_FREE;
	current_behavior = PATROL;
	near_player = -1;
	distance_near = 100000.f;
	my_sector = -1;
}

void NPC::WakeUp(int p_id)
{
	near_player = p_id;

	if (is_active) return;
	bool old_state = false;
	if (false == atomic_compare_exchange_strong(&is_active, &old_state, true))
		return;
	// std::cout << "NPC[" << id << "] WakeUp  By P[" << near_player << "] " << std::endl;
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
	if (IsAttack) return;
	if (n_path.empty())
		return;

	// std::cout << "NPC[" << id << "] " << current_behavior<< " Move " << GetPos().x << ", " << GetPos().z << std::endl;
	
	SetPos(n_path.front());
	n_path.pop();

}

void NPC::Patrol()
{
	if (n_path.empty()) {
		return;
	}
	DirectX::XMFLOAT3 next = n_path.front();
	// std::cout << "[" << id << "] Patrol" << std::endl;
	
	TIMER_EVENT ev{ id, near_player, std::chrono::system_clock::now()+std::chrono::milliseconds(240), EV_NPC_MOVE};
	auto& server = Server::GetInstance();
	server.timer_queue.push(ev);

	for (auto cl : server.gMap.cl_ids) {
		server.clients[cl].send_move_npc_packet(id, next);
	}

}

void NPC::Chase()
{
	if (n_path.empty()) {
		return;
	}
	DirectX::XMFLOAT3 next = n_path.front();
	TIMER_EVENT ev{ id, near_player, std::chrono::system_clock::now() + std::chrono::milliseconds(240), EV_NPC_MOVE };
	auto& server = Server::GetInstance();
	server.timer_queue.push(ev);
	

	for (auto cl : server.gMap.cl_ids) {
		server.clients[cl].send_move_npc_packet(id,next);
	}
}

void NPC::Attack()
{
	o_lock.lock();
	IsAttack = true;
	o_lock.unlock();
	PathClear();
	std::cout << "[" << id << "] Attack½ÃÀÛ [" << near_player << "] " << std::endl;
	auto& server = Server::GetInstance();
	for (int id : server.gMap.cl_ids) {
		SC_ATTACK_NPC_PACKET p;
		p.size = sizeof(p);
		p.type = SC_ATTACK_NPC;
		p.n_id = GetId();
		p.p_id = near_player;
		server.clients[id].do_send(&p);
	}

	
	TIMER_EVENT ev_cool{ id, near_player, std::chrono::system_clock::now() + std::chrono::milliseconds(cool_time), EV_COOL_DOWN };
	
	TIMER_EVENT ev_atk{ id, near_player, std::chrono::system_clock::now() + std::chrono::milliseconds(900), EV_NPC_ATTACK };
	
	server.timer_queue.push(ev_cool);
	server.timer_queue.push(ev_atk);

}

void NPC::PathClear()
{
	std::queue<DirectX::XMFLOAT3> q{};
	n_path = q;
	
}

void NPC::UpdateBB()
{
	boundingBox.Center = GetPos();
}




