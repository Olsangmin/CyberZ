#pragma once
#include "Object.h"
#include "Timer.h"
#include <queue>

enum NPC_STATE {NPC_FREE, NPC_INGAME};



class NPC : public Object
{
public:
	NPC();
	
	void WakeUp(int p_id);

	void DoWork();

	void Move();

	void Patrol();
	void Chase();
	void Attack();

	

public:
	NPC_STATE n_state;

	NPC_BEHAVIOR current_behavior;
	
	int my_sector;

	int near_player;
	float distance_near;

	std::atomic_bool is_active;
	std::queue<DirectX::XMFLOAT3> n_path;
};

