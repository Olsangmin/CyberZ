#pragma once
#include "Common.h"

class Object
{
public:
	Object() : id(-1){}

	void SetId(int _id) { id = _id; }
	int GetId() const { return id; }
	
	Position GetPos() const {return pos;}
	void SetPos(Position p) { pos.x = p.x; pos.y = p.y; }
	
protected:
	int		id;
	Position		pos;
	
public:
	std::mutex o_lock;
};

