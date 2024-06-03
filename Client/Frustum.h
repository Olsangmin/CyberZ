#include "stdafx.h"
#pragma once

enum PLANE_TYPE : uint8_t
{
	PLANE_FRONT,
	PLANE_BACK,
	PLANE_UP,
	PLANE_DOWN,
	PLANE_LEFT,
	PLANE_RIGHT,

	PLANE_END
};

class Frustum
{
public:
	void FinalUpdate();
	bool ContainsSphere(XMFLOAT3& pos, float radius);

private:
	array<XMFLOAT4, PLANE_END> _planes;
};