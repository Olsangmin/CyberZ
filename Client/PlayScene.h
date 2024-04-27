//-----------------------------------------------------------------------------
// File: PlayScene.h
//-----------------------------------------------------------------------------

#pragma once
#include "Scene.h"
#include "Object.h"

class PlayScene :public CScene
{
public:
	PlayScene(){}
	~PlayScene() {}

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int myPlayernum);
	bool ProcessInput(HWND m_hWnd, POINT m_ptOldCursorPos, UCHAR* pKeysBuffer);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	bool AllPlayerReady() { return false; }

	void ProcessPacket(char* p);
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

	int								m_nPlayerSelecter = 0;
	CSelectCharacterOBJ**			m_ppPlayerSelecter = NULL;

public:
	ID3D12Device*					pScened3dDevice;
	ID3D12GraphicsCommandList*		pScened3dCommandList;

	int								m_nOnPlayer = NULL;
	CSelectCharacterOBJ**			m_ppOnPlayer = NULL;
};
