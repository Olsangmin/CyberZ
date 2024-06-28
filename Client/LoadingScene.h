
//-----------------------------------------------------------------------------
// File: LoadingScene.h
//-----------------------------------------------------------------------------


#pragma once
#include "Scene.h"
#include "Object.h"
#include <unordered_map>
#include <algorithm>

class CLoadingScene : public CScene
{
public:
	CLoadingScene() { m_bChangeScene = true; }
	~CLoadingScene() {}

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int myPlayernum = 4);

	bool ProcessInput(HWND m_hWnd, POINT m_ptOldCursorPos, UCHAR* pKeysBuffer);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual bool AllPlayerReady() { return true; }


public:
	int m_nPlayerSet[3] = { 0 };
	
	void ProcessPacket(char* p) {}
	void SetModelInfo();
	int GetModelInfo();

};

