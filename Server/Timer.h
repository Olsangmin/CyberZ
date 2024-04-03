#pragma once
#include <chrono>

enum Timer_EVENT_TYPE { EV_NPC_MOVE };

struct TIMER_EVENT
{
	int npc_id;
	int pl_id;
	std::chrono::system_clock::time_point wakeup_time;
	Timer_EVENT_TYPE ev_type;

	constexpr bool operator < (const TIMER_EVENT& L) const
	{
		return (L.wakeup_time < wakeup_time);
	}
}; 

