#pragma once
#include "Common.h"

class Object
{
public:
	Object();

	void SetId(int _id) { id = _id; }
	int GetId() const { return id; }
	
	Position GetPos() const {return pos;}
	void SetPos(Position p) { pos.x = p.x; pos.y = p.y; }
	
	float GetYaw() const { return yaw; }
	void SetYaw(float _yaw) { yaw = _yaw; }

protected:
	int			id;
	Position		pos;
	float		yaw;
	
public:
	std::mutex o_lock;
};

