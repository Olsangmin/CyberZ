#pragma once
#include "Object.h"
#include "Timer.h"

enum NPC_STATE {NPC_FREE, NPC_INGAME, NPC_STAY};

class NPC :public Object
{
public:
	NPC();
	
	void WakeUp(int p_id);

public:
	NPC_STATE n_state;
	int near_player;
};

