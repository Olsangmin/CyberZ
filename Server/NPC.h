#pragma once
#include "Object.h"
#include "Timer.h"
#include <queue>

enum NPC_STATE {NPC_FREE, NPC_INGAME, NPC_STAY, NPC_MOVE, NPC_PATROL};

class NPC : public Object
{
public:
	NPC();
	
	void WakeUp(int p_id);

	void Update();

	DirectX::XMFLOAT3 Move();

	bool NeedPath();

public:
	NPC_STATE n_state;
	
	int my_sector;

	int near_player;
	std::atomic_bool is_active;
	std::queue<DirectX::XMFLOAT3> n_path;
};

