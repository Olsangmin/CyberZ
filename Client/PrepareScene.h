//-----------------------------------------------------------------------------
// File: PrePareScene.h
//-----------------------------------------------------------------------------

#pragma once
#include "Scene.h"
#include "Object.h"
#include <unordered_map>
#include <algorithm>
#include <wincrypt.h>

#include <locale>
#include <codecvt>


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

	std::wstring string_to_wstring(const std::string& str) {
		int len;
		int strLength = static_cast<int>(str.length()) + 1;
		len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), strLength, 0, 0);
		std::wstring wstr(len, L'\0');
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), strLength, &wstr[0], len);
		return wstr;
	}

public:
	void ProcessPacket(char* p);
	int	GetModelInfo();
	int GetPlayerModelInfo(int num) { return m_nPlayerSet[num]; }

	int m_nPlayerSet[3] = { 0 };
	Player_Character_Type select{ Robot };

};
