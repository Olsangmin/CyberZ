
#include "stdafx.h"
#include "PlayScene.h"

void PlayScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CScene::BuildObjects(pd3dDevice, pd3dCommandList);

	//===============================//
	// SKY BOX (1)
	m_pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	//===============================//
	// TERRAIN (1)
	XMFLOAT3 xmf3Scale(15.0f, 1.0f, 15.0f);
	XMFLOAT4 xmf4Color(0.2f, 0.2f, 0.2f, 0.0f);
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("Terrain/BaseTerrain.raw"), 257, 257, xmf3Scale, xmf4Color);
	m_pTerrain->SetPosition(XMFLOAT3(-2000.f, 0.f, -2000.f));

	//===============================//
	// OBJ (1)
	// [Present Setting]
	// 1		- map				|| OBJ

	m_nHierarchicalGameObjects = 1;
	m_ppHierarchicalGameObjects = new CGameObject * [m_nHierarchicalGameObjects];

	// 1 - obj1
	CLoadedModelInfo* pMapModle1 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/test/fence.bin", NULL);
	m_ppHierarchicalGameObjects[0] = new CStandardOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pMapModle1);
	m_ppHierarchicalGameObjects[0]->SetPosition(100.f, 0.f, 100.f);


	if (pMapModle1) delete pMapModle1;

	//===============================//

	m_nEnemy = 3;
	m_ppEnemy = new CGameObject * [m_nEnemy];

	for (int i = 0; i < m_nEnemy; i++)
	{
		// 0 - Robot
		CLoadedModelInfo* pRobotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Robot.bin", NULL);
		m_ppEnemy[i] = new CRobotObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pRobotModel, 1);
		m_ppEnemy[i]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
		m_ppEnemy[i]->SetPosition(0.0f, 0, 0.0f + i * 100);
		m_ppEnemy[i]->SetScale(10.f, 10.f, 10.f);
	
		if (pRobotModel) delete pRobotModel;
	}

	//===============================//
	// SHADER OBJ (NULL)

	m_nShaders = 0;
	/*
		m_ppShaders = new CShader*[m_nShaders];

		CEthanObjectsShader *pEthanObjectsShader = new CEthanObjectsShader();
		pEthanObjectsShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pEthanModel, m_pTerrain);

		m_ppShaders[0] = pEthanObjectsShader;
		if (pEthanModel) delete pEthanModel;
	*/

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	//===============================//
	// Player (3 / 1 - Corzim, 2 - Evan, 3 - Uranya)
	// [Present Setting]
	// 1	- Corzim (Player)
	// 2	- Evan
	// 3	- Corzim
	// Able Model - Evam, Corzim
	// Unable Model - Uranya

	m_nPlayer = MAX_PLAYER;

	m_ppPlayer = new CPlayer * [m_nPlayer];

	m_ppModelInfoPlayer = new CLoadedModelInfo * [m_nPlayer];

	// 저장된 모델 바꿀 수 있음
	m_ppModelInfoPlayer[FIRST_PLAYER] = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), "Model/Player_1.bin", NULL);
	m_ppModelInfoPlayer[SECOND_PLAYER] = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), "Model/Player_2.bin", NULL);
	m_ppModelInfoPlayer[THIRD_PLAYER] = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), "Model/Player_3.bin", NULL);

	for (int i = 0; i < m_nPlayer; ++i) {
		CyborgPlayer* pPlayer = new CyborgPlayer(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), m_pTerrain, m_ppModelInfoPlayer[i]);
		m_ppPlayer[i] = pPlayer;
		m_ppPlayer[i]->SetPlayerData(i);
	}
	m_pMyPlayer = m_ppPlayer[MY_PLAYER];
}

bool PlayScene::ProcessInput(HWND m_hWnd, POINT m_ptOldCursorPos, UCHAR* pKeysBuffer)
{
	CScene::ProcessInput(m_hWnd, m_ptOldCursorPos, pKeysBuffer);
	
	float cxDelta = 0.0f, cyDelta = 0.0f;
	POINT ptCursorPos;
	if (GetCapture() == m_hWnd)
	{
		SetCursor(NULL);
		GetCursorPos(&ptCursorPos);
		cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
		SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
	}

	DWORD dwDirection = 0;
	DWORD dwDirection1 = 0;

	if (pKeysBuffer['W'] & 0xF0) dwDirection1 |= DIR_FORWARD;
	if (pKeysBuffer['S'] & 0xF0) dwDirection1 |= DIR_BACKWARD;
	if (pKeysBuffer['A'] & 0xF0) dwDirection1 |= DIR_LEFT;
	if (pKeysBuffer['D'] & 0xF0) dwDirection1 |= DIR_RIGHT;

	// Player unable
	// m_ppPlayer[원하는 캐릭터]->m_bUnable = true 면은 ADD_OBJ(실제로 생성이 아닌 렌더&움직임 가능 상태)
	if (pKeysBuffer['1'] & 0xF0) m_ppPlayer[FIRST_PLAYER]->m_bUnable = true;
	if (pKeysBuffer['2'] & 0xF0) m_ppPlayer[SECOND_PLAYER]->m_bUnable = true;
	if (pKeysBuffer['3'] & 0xF0) m_ppPlayer[THIRD_PLAYER]->m_bUnable = true;

	// Player disable
	if (pKeysBuffer['4'] & 0xF0) m_ppPlayer[FIRST_PLAYER]->m_bUnable = false;
	if (pKeysBuffer['5'] & 0xF0) m_ppPlayer[SECOND_PLAYER]->m_bUnable = false;
	if (pKeysBuffer['6'] & 0xF0) m_ppPlayer[THIRD_PLAYER]->m_bUnable = false;

	// Decide whether to blend
	// 전에 입력한 키와 다르다면 블렌딩타임을 0으로 설정
	// 블렌딩 타임이 0 -> 블렌딩 시작
	// 블렌딩 타임이 1 -> 블렌딩 완료

	//if (m_dwLastDirection != dwDirection1) {
	//	m_pMyPlayer->m_pSkinnedAnimationController->m_fBlendingTime = 0.0f;
	//}

	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f) || (dwDirection1 != 0))
	{
		if ((cxDelta || cyDelta) && m_pMyPlayer->m_bUnable)
			m_pMyPlayer->CameraRotate(0.0f, cxDelta, 0.0f);

		if (dwDirection1 && m_pMyPlayer->m_bUnable)
			m_pMyPlayer->Move(dwDirection1, reinterpret_cast<CyborgPlayer*>(m_pMyPlayer)->m_fVelocitySpeed, true);
	}

	m_dwLastDirection = dwDirection1;

	return(false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
void CPrepareRoomScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CScene::BuildObjects(pd3dDevice, pd3dCommandList);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	//===============================//
	// TERRAIN
	XMFLOAT3 xmf3Scale(0.0001f, 0.0001f, 0.0001f);
	XMFLOAT4 xmf4Color(0.0f, 0.1f, 0.1f, 0.0f);
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("Terrain/BaseTerrain.raw"), 257, 257, xmf3Scale, xmf4Color);

	//===============================//
	// OBG
	m_nHierarchicalGameObjects = 0;
	m_ppHierarchicalGameObjects = new CGameObject * [m_nHierarchicalGameObjects];

	//===============================//
	// Player
	m_nPlayer = MAX_PLAYER + 1;
	m_ppPlayer = new CPlayer * [m_nPlayer];
	m_ppModelInfoPlayer = new CLoadedModelInfo * [m_nPlayer]; // Play Character

	// 저장된 모델 바꿀 수 있음
	m_ppModelInfoPlayer[FIRST_PLAYER] = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), "Model/Player_1.bin", NULL);
	m_ppModelInfoPlayer[SECOND_PLAYER] = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), "Model/Player_2.bin", NULL);
	m_ppModelInfoPlayer[THIRD_PLAYER] = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), "Model/Player_3.bin", NULL);
	m_ppModelInfoPlayer[TEMP_PLAYER] = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), "Model/robot.bin", NULL);


	for (int i = 0; i < m_nPlayer; ++i) {
		CyborgPlayer* pPlayer = new CyborgPlayer(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), m_pTerrain, m_ppModelInfoPlayer[i]);
		m_ppPlayer[i] = pPlayer;
		m_ppPlayer[i]->Rotate(0.f, 180.f, 0.f);
	}

	for (int i = 0; i < m_nPlayer; ++i) m_ppPlayer[i]->SetPlayerData(i);
	
	m_pMyPlayer = m_ppPlayer[TEMP_PLAYER];
	m_pMyPlayer->m_bUnable = true;
	m_pMyPlayer->ChangeCamera(PREPARE_ROOM_CAMERA, 0.0f);
}



bool CPrepareRoomScene::ProcessInput(HWND m_hWnd, POINT m_ptOldCursorPos, UCHAR* pKeysBuffer)
{
	CScene::ProcessInput(m_hWnd, m_ptOldCursorPos, pKeysBuffer);

	float cxDelta = 0.0f, cyDelta = 0.0f;
	POINT ptCursorPos;
	if (GetCapture() == m_hWnd)
	{
		SetCursor(NULL);
		GetCursorPos(&ptCursorPos);
		cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
		SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
	}

	// Player unable
	// m_ppPlayer[원하는 캐릭터]->m_bUnable = true 면은 ADD_OBJ(실제로 생성이 아닌 렌더&움직임 가능 상태)
	//if (pKeysBuffer['1'] & 0xF0) m_ppPlayer[FIRST_PLAYER]->m_bUnable = true;
	//if (pKeysBuffer['2'] & 0xF0) m_ppPlayer[SECOND_PLAYER]->m_bUnable = true;
	//if (pKeysBuffer['3'] & 0xF0) m_ppPlayer[THIRD_PLAYER]->m_bUnable = true;
	//if (pKeysBuffer['4'] & 0xF0) m_ppPlayer[TEMP_PLAYER]->m_bUnable = true;
	//
	//// Player disable
	//if (pKeysBuffer['5'] & 0xF0) m_ppPlayer[FIRST_PLAYER]->m_bUnable = false;
	//if (pKeysBuffer['6'] & 0xF0) m_ppPlayer[SECOND_PLAYER]->m_bUnable = false;
	//if (pKeysBuffer['7'] & 0xF0) m_ppPlayer[THIRD_PLAYER]->m_bUnable = false;
	//if (pKeysBuffer['8'] & 0xF0) m_ppPlayer[TEMP_PLAYER]->m_bUnable = false;
	
	if (pKeysBuffer['9'] & 0xF0) m_pMyPlayer->m_bUnable = true;
	if (pKeysBuffer['0'] & 0xF0) m_pMyPlayer->m_bUnable = false;

	// change Character
	if (pKeysBuffer['1'] & 0xF0 || pKeysBuffer['2'] & 0xF0 || pKeysBuffer['3'] & 0xF0 || pKeysBuffer['4'] & 0xF0)
	{
		m_pMyPlayer->m_bUnable = false;
		if (pKeysBuffer['1'] & 0xF0)m_pMyPlayer = m_ppPlayer[FIRST_PLAYER];
		if (pKeysBuffer['2'] & 0xF0)m_pMyPlayer = m_ppPlayer[SECOND_PLAYER];
		if (pKeysBuffer['3'] & 0xF0)m_pMyPlayer = m_ppPlayer[THIRD_PLAYER];
		if (pKeysBuffer['4'] & 0xF0)m_pMyPlayer = m_ppPlayer[TEMP_PLAYER];
		m_pMyPlayer->m_bUnable = true;
	}

	if (pKeysBuffer['r'] & 0xF0) m_pMyPlayer->m_bReady = !m_pMyPlayer->m_bReady;

	return(false);
}

