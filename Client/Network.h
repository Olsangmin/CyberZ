#pragma once
#include "stdafx.h"

constexpr int BUF_SIZE = 4096;
constexpr int INGAME_USER = 3;
constexpr int MAX_NPC = 10;

extern SOCKET c_socket;
//static string ServerIP = { "49.173.80.42" };
//static string ServerIP = {"127.0.0.1"};
// static string ServerIP = {"192.168.219.162"};

extern int my_id;

enum Obj_STATE{ FREE, INGAME };
class ObjectInfo {
public:
	ObjectInfo() : id(-1), pos{}, yaw{}, state(FREE) {}
public:
	int id;
	DirectX::XMFLOAT3 pos;
	float yaw;
	Obj_STATE state;
};


