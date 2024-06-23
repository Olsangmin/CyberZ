//-----------------------------------------------------------------------------
// File: PlayScene.h
//-----------------------------------------------------------------------------

#pragma once
#include "Scene.h"
#include "Object.h"

class CStartScene :public CScene
{
public: 
	CStartScene() {}
	~CStartScene() {}

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int myPlayernum = 4);

	void ProcessPacket(char* p);


};

