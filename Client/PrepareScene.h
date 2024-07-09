//-----------------------------------------------------------------------------
// File: PrePareScene.h
//-----------------------------------------------------------------------------

#pragma once
#include "Scene.h"
#include "Object.h"
#include <unordered_map>
#include <algorithm>
#include <wincrypt.h>


class CPrepareRoomScene : public CScene
{
public:
	CPrepareRoomScene() { m_bChangeScene = false; }
	~CPrepareRoomScene() {}

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int myPlayernum);

	void ReleaseObjects();
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
	void ReleaseUploadBuffers();

	bool ProcessInput(HWND m_hWnd, POINT m_ptOldCursorPos, UCHAR* pKeysBuffer);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	bool AllPlayerReady();
	void SetPlayer();

	void ChangeModel(int nPlayer, int nModel);

public:
	void ProcessPacket(char* p);
	int	GetModelInfo();
	int GetPlayerModelInfo(int num) { return m_nPlayerSet[num]; }

	int m_nPlayerSet[3] = { 0 };
	Player_Character_Type select{ Robot };

};
