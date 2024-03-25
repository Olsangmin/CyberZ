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

	DirectX::XMFLOAT3 GetRotation() const { return rotation; }
	void SetRotation(DirectX::XMFLOAT3 _rotation) { rotation = _rotation; }

protected:
	int			id;
	DirectX::XMFLOAT3		pos;
	DirectX::XMFLOAT3		rotation;

public:
	std::mutex o_lock;
};

