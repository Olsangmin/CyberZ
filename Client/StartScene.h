//-----------------------------------------------------------------------------
// File: StartScene.h
//-----------------------------------------------------------------------------

#pragma once
#include "Scene.h"
#include "Object.h"
#include <unordered_map>
#include <algorithm>

class CStartScene :public CScene
{
public: 
	CStartScene() { m_bChangeScene = false; }
	~CStartScene() {}

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int myPlayernum = 4);
	virtual void ReleaseObjects();

	// Input
	bool ProcessInput(HWND m_hWnd, POINT m_ptOldCursorPos, UCHAR* pKeysBuffer);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void Send_Login();
	void Send_SignUp();

private:
	const int					TextMax = 17;

	POINT						m_ptOldCursorPos;

	bool						m_bInputID = false;
	bool						m_bInputPW = false;
	bool						m_bSignUP = false;
	bool						m_bLogin = false;

	std::string				m_ID = "ID";
	std::string				m_PW = "Pw";


public:
	void ProcessPacket(char* p)
	{
		switch (p[1])
		{
		case SC_LOGIN_INFO: 
		{
			SC_LOGIN_INFO_PACKET* packet = reinterpret_cast<SC_LOGIN_INFO_PACKET*>(p);
			my_id = packet->id;
			cout << "My ID is " << my_id << " !" << endl;
			// reinterpret_cast<CFirstSceneUI*>(m_pUI)->m_bPlayerOn[my_id] = true;
			m_bChangeScene = true;
		} break;


		default: {
			printf("Scene[StartScene] - Unknown PACKET type [%d]\n", p[1]);
			break;
		}
		}
	}
};

