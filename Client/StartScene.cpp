
#include "stdafx.h"
#include "StartScene.h"

void CStartScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int myPlayernum)
{
	CScene::BuildObjects(pd3dDevice, pd3dCommandList, myPlayernum);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	m_pUI = new CStartSceneUI();


	//===============================//
	// SKY BOX (1)
	m_pSkyBox = NULL;


	//===============================//
	// TERRAIN
	XMFLOAT3 xmf3Scale(10.f, 10.f, 10.f);
	XMFLOAT4 xmf4Color(0.0f, 0.0f, 0.0f, 0.0f);
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("Terrain/BaseTerrain.raw"), 257, 257, xmf3Scale, xmf4Color);
	m_pTerrain->SetPosition(-1000.f, 0, -1000.f);

	//===============================//
	m_nHierarchicalGameObjects = 0;
	m_nMissionObj = 0;
	m_nEnemy = 0;
	m_nShaders = 0;

	//===============================//
	// Player (3 / 1 - Corzim, 2 - Evan, 3 - Uranya)
	// [Present Setting]
	// 1	- Corzim (Player)
	// 2	- Evan
	// 3	- Corzim
	// Able Model - Evam, Corzim
	// Unable Model - Uranya

	//===============================//
	// Player
	m_nPlayer = 1;
	m_ppPlayer = new CPlayer * [m_nPlayer];
	m_ppModelInfoPlayer = new CLoadedModelInfo * [m_nPlayer]; // Play Character

	// ÀúÀåµÈ ¸ðµ¨ ¹Ù²Ü ¼ö ÀÖÀ½
	m_ppModelInfoPlayer[FIRST_PLAYER] = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), "Model/Player_1.bin", NULL);

	for (int i = 0; i < m_nPlayer; ++i) {
		CyborgPlayer* pPlayer = new CyborgPlayer(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), m_pTerrain, m_ppModelInfoPlayer[i]);
		m_ppPlayer[i] = pPlayer;
		m_ppPlayer[i]->SetPlayerData(i);
	}

	m_pMyPlayer = m_ppPlayer[MY_PLAYER];
	m_pMyPlayer->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_pMyPlayer->ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);
	m_pMyPlayer->m_bUnable = false;
}

bool CStartScene::ProcessInput(HWND m_hWnd, POINT m_ptOldCursorPos, UCHAR* pKeysBuffer)
{
	CScene::ProcessInput(m_hWnd, m_ptOldCursorPos, pKeysBuffer);
	return(false);

}

bool CStartScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{

	if (m_bInputID){
		switch (nMessageID)
		{
		case WM_KEYDOWN: {
			switch (wParam) {
			case VK_BACK:
			{
				if (!reinterpret_cast<CStartSceneUI*>(m_pUI)->m_ID.empty())
				{
					reinterpret_cast<CStartSceneUI*>(m_pUI)->m_ID.pop_back();
				}
				break;
			}
			default:
				reinterpret_cast<CStartSceneUI*>(m_pUI)->m_ID += wParam;
				break;
			}
		}
		}
		
	}
	else if (m_bInputPW) {
		switch (nMessageID)
		{
		case WM_KEYDOWN: {
			switch (wParam) {
			case VK_BACK:
			{
				if (!reinterpret_cast<CStartSceneUI*>(m_pUI)->m_PW.empty())
				{
					reinterpret_cast<CStartSceneUI*>(m_pUI)->m_PW.pop_back();
				}
				break;
			}
			default:
				reinterpret_cast<CStartSceneUI*>(m_pUI)->m_PW += wParam;
				break;
			}
		}
		}

	}
	else
	{
		switch (nMessageID)
		{
		case WM_KEYDOWN: {
			switch (wParam) {

			default:
				break;
			}


		}
		case WM_KEYUP: {
			switch (wParam) {
			case 'A':
				std::cout << "Change Scene" << endl;
				m_bChangeScene = true;
				break;

			default:
				break;
			}
		}
		}
	}
	CScene::OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
	return(false);

}

bool CStartScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);

		m_bInputID = reinterpret_cast<CStartSceneUI*>(m_pUI)->m_ppTextInputBox[0]->CheckChlick(hWnd, m_ptOldCursorPos);
		m_bInputPW = reinterpret_cast<CStartSceneUI*>(m_pUI)->m_ppTextInputBox[1]->CheckChlick(hWnd, m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
	return(true);

}
