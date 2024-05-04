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



void NPC::Update()
{
	if (n_state != NPC_INGAME) return;

	if (current_behavior == next_behavior) return;

	current_behavior = next_behavior;

	switch (next_behavior)
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
	

	// std::cout << "NPC[" << id << "] - (" << GetPos().x << ", " << GetPos().z << "). " << std::endl;
}

DirectX::XMFLOAT3 NPC::Move()
{
	if (n_path.empty())
		return GetPos();

	DirectX::XMFLOAT3 next = n_path.front();
	std::cout << "NPC[" << id << "] " << current_behavior<< " Move " << next.x << ", " << next.z << std::endl;
	SetPos(next);
	n_path.pop();

	return next;
}

void NPC::Patrol()
{
	if (false == n_path.empty()) {
		return;
	}

	if (is_active) return;
	bool old_state = false;
	if (false == atomic_compare_exchange_strong(&is_active, &old_state, true))
		return;

	TIMER_EVENT ev{ id, near_player, std::chrono::system_clock::now(), EV_NPC_MOVE };
	auto& server = Server::GetInstance();
	server.timer_queue.push(ev);
}

void NPC::Chase()
{
	if (n_path.empty()) {
		next_behavior = PATROL;
		return;
	}

	if (is_active) return;
	bool old_state = false;
	if (false == atomic_compare_exchange_strong(&is_active, &old_state, true))
		return;

	TIMER_EVENT ev{ id, near_player, std::chrono::system_clock::now(), EV_NPC_MOVE };
	auto& server = Server::GetInstance();
	server.timer_queue.push(ev);
}

void NPC::Attack()
{

}


