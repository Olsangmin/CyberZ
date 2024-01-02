#pragma once
#include "Common.h"

class Object
{
public:
	Object() : id(-1){}

protected:
	int		id;

public:
	std::mutex o_lock;
};

