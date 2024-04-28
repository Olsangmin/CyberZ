#pragma once
#include "../Common.h"
#include "../Protocol.h"
#include <atomic>

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

	Player_Character_Type GetType() const { return type; }
	void SetType(Player_Character_Type _type) { type = _type; }

protected:
	int			id;
	DirectX::XMFLOAT3		pos;
	DirectX::XMFLOAT3		rotation;
	
	Player_Character_Type type;

public:
	std::mutex o_lock;
};

