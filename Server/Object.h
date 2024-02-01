#pragma once
#include "Common.h"

class Object
{
public:
	Object() : id(-1){}

	void SetId(int _id) { id = _id; }
	float GetF() { return f; }
	void SetF(float i) { f = i; }
protected:
	int		id;
	int		x, y;
	float	f; // float3 pos·Î ¹Ù²ã¾ßµÊ.
public:
	std::mutex o_lock;
};

