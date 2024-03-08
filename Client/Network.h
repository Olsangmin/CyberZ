#pragma once
#include "stdafx.h"
#include "Common.h"
#include "Protocol.h"

constexpr int BUF_SIZE = 4096;
constexpr int INGAME_USER = 3;
constexpr int MAX_NPC = 10;

extern SOCKET c_socket;
static string ServerIP = { "127.0.0.1" };
extern int my_id;

enum Obj_STATE{ FREE, INGAME};
class ObjectInfo {
public:
	ObjectInfo() : id(-1), pos{}, yaw{}, state(FREE) {}
public:
	int id;
	DirectX::XMFLOAT3 pos;
	float yaw;
	Obj_STATE state;
};



