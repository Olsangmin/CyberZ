#pragma once
#include "../Common.h"
#include "../Protocol.h"

class Object
{
public:
	Object();

	void SetId(int _id) { id = _id; }
	int GetId() const { return id; }

	DirectX::XMFLOAT3 GetPos() const { return pos; }
	void SetPos(DirectX::XMFLOAT3 p) { pos = p; }

	float GetYaw() const { return yaw; }
	void SetYaw(float _yaw) { yaw = _yaw; }

protected:
	int			id;
	DirectX::XMFLOAT3		pos;
	float		yaw;

public:
	std::mutex o_lock;
};

