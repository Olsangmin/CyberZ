#pragma once
#include "Object.h"
#include "Timer.h"
#include <queue>

enum NPC_STATE {NPC_FREE, NPC_INGAME, NPC_STAY};

class NPC : public Object
{
public:
	NPC();
	
	void WakeUp(int p_id);

	void Update();

public:
	NPC_STATE n_state;
	int near_player;
	std::atomic_bool is_active;
	std::queue<DirectX::XMFLOAT3> n_path;
};

