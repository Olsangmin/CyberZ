//-----------------------------------------------------------------------------
// File: PlayScene.h
//-----------------------------------------------------------------------------

#pragma once
#include "Scene.h"

class PlayScene :public CScene
{
public:
	PlayScene(){}
	~PlayScene() {}

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	bool ProcessInput(HWND m_hWnd, POINT m_ptOldCursorPos, UCHAR* pKeysBuffer);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CPrepareRoomScene : public CScene
{
public:
	CPrepareRoomScene() {}
	~CPrepareRoomScene() {}

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	bool ProcessInput(HWND m_hWnd, POINT m_ptOldCursorPos, UCHAR* pKeysBuffer);
};
