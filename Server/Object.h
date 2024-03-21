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

	float GetVelocity() const { return velocity; }
	void SetVelocity(float _velocity) { velocity = _velocity; }

protected:
	int			id;
	DirectX::XMFLOAT3		pos;
	float		yaw;
	float		velocity;

public:
	std::mutex o_lock;
};

