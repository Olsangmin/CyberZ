//-----------------------------------------------------------------------------
// File: PlayScene.h
//-----------------------------------------------------------------------------

#pragma once
#include "Scene.h"
#include "Object.h"
#include <unordered_map>
#include <algorithm>
#include <wincrypt.h>

class CFirstRoundScene :public CScene
{
public:
	CFirstRoundScene() { m_bChangeScene = false; }
	~CFirstRoundScene() {}

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int myPlayernum = 4);
	
	void ReleaseObjects();
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
	void ReleaseUploadBuffers();

	bool ProcessInput(HWND m_hWnd, POINT m_ptOldCursorPos, UCHAR* pKeysBuffer);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void AnimateObjects(float fTimeElapsed);

	bool AllPlayerReady() { return false; }


public:
	void ProcessPacket(char* p);
	unordered_map<int, Player_Character_Type> idANDtype;
	void Send_Go_Stage2();

};

class CSecondRoundScene : public CScene
{
public:
	CSecondRoundScene() { m_bChangeScene = false; }
	~CSecondRoundScene() {}


	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int myPlayernum = 4);

	void ReleaseObjects();
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
	void ReleaseUploadBuffers();

	bool ProcessInput(HWND m_hWnd, POINT m_ptOldCursorPos, UCHAR* pKeysBuffer);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void AnimateObjects(float fTimeElapsed);

public:

	POINT						m_ptOldCursorPos;

	int							m_nFloorObj = 0;
	CFloorObj					**m_ppFloorObj = NULL;

	int							m_nMissionLevel = 0;
	bool						m_nMissionOn = true;
	

public:
	void ProcessPacket(char* p);
	unordered_map<int, Player_Character_Type> idANDtype;


};