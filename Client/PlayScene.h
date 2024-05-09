//-----------------------------------------------------------------------------
// File: PlayScene.h
//-----------------------------------------------------------------------------

#pragma once
#include "Scene.h"
#include "Object.h"
#include <unordered_map>
#include <algorithm>

class CPlayScene :public CScene
{
public:
	CPlayScene(){}
	~CPlayScene() {}

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int myPlayernum = 4);
	
	void ReleaseObjects();
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
	void ReleaseUploadBuffers();

	bool ProcessInput(HWND m_hWnd, POINT m_ptOldCursorPos, UCHAR* pKeysBuffer);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void AnimateObjects(float fTimeElapsed);

	bool AllPlayerReady() { return false; }

public:
	int						m_nFloorObj = 0;
	CFloorObj**				m_ppFloorObj = NULL;

public:
	void ProcessPacket(char* p);
	unordered_map<int, Player_Character_Type> idANDtype;

	
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CPrepareRoomScene : public CScene
{
public:
	CPrepareRoomScene() {}
	~CPrepareRoomScene() {}

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int myPlayernum);
	bool ProcessInput(HWND m_hWnd, POINT m_ptOldCursorPos, UCHAR* pKeysBuffer);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void ReleaseObjects();
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
	void ReleaseUploadBuffers();

	bool AllPlayerReady();

	void SetChangedModel(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ChangeModel(int nPlayer, int nModel);
	
	void ProcessPacket(char* p);
	int getModelInfo();

public:

	Player_Character_Type select{ Robot };

	int								m_nPlayerSelecter = 0;
	CSelectCharacterOBJ**			m_ppPlayerSelecter = NULL;

public:	
	CSelectCharacterOBJ**			m_ppOnPlayer = NULL;
};
