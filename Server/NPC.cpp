#include "NPC.h"
#include "Server.h"

NPC::NPC()
{
	id = -2;
	n_state = NPC_FREE;
	near_player = -1;
}

void NPC::WakeUp(int p_id)
{
	near_player = p_id;
	
	std::cout << "NPC[" << id << "] Move to Player[" << near_player << "] " << std::endl;

	TIMER_EVENT ev{ id, near_player, std::chrono::system_clock::now() + std::chrono::seconds(3), EV_NPC_MOVE };

	auto& server = Server::GetInstance();
	server.timer_queue.push(ev);
	// timer_queue.push(ev);
}
