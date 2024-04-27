#include "NPC.h"

NPC::NPC()
{
	id = -2;
	n_state = NPC_FREE;
	near_player = -1;
}

void NPC::WakeUp(int p_id)
{
	near_player = p_id;

	if (is_active) return;
	bool old_state = false;
	if (false == atomic_compare_exchange_strong(&is_active, &old_state, true))
		return;
	std::cout << "NPC[" << id << "] Move to Player[" << near_player << "] " << std::endl;
	//TIMER_EVENT ev{ id, near_player, std::chrono::system_clock::now() + std::chrono::seconds(3), EV_NPC_MOVE };
	TIMER_EVENT ev{ id, near_player, std::chrono::system_clock::now(), EV_NPC_MOVE };

	/*auto& server = Server::GetInstance();
	server.timer_queue.push(ev);*/
	// timer_queue.push(ev);

}



void NPC::Update()
{
	int state = n_state;
	switch (state)
	{
	case NPC_INGAME: {

	} break;

	case NPC_STAY: {

	} break;

	case NPC_MOVE: {

	}




	default:
		break;
	}
	if (n_path.empty())
		return;
	DirectX::XMFLOAT3 next = n_path.front();
	SetPos(next);
	n_path.pop();
	std::cout << "NPC[" << id << "] - (" << GetPos().x << ", " << GetPos().z << "). " << std::endl;
}
