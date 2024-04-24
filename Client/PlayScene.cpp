
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
	// 1 - map				|| OBJ

	m_nHierarchicalGameObjects = 2;
	m_ppHierarchicalGameObjects = new CGameObject * [m_nHierarchicalGameObjects];

	// 1 - obj1
	CLoadedModelInfo* pfenceModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/test/fence.bin", NULL);
	m_ppHierarchicalGameObjects[0] = new CStandardOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pfenceModel);
	m_ppHierarchicalGameObjects[0]->SetScale(0.01f, 0.01f, 0.01f);

	
	CLoadedModelInfo* pMapModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/map/MiddleCheckMap.bin", NULL);
	m_ppHierarchicalGameObjects[1] = new CStandardOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pMapModel);
	//m_ppHierarchicalGameObjects[1]->SetScale(0.01f, 0.01f, 0.01f);
	

	//CLoadedModelInfo* pMapModle2 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/map/MiddleCheckMap.bin", NULL);
	//m_ppHierarchicalGameObjects[2] = new CStandardOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pMapModle2);

	if (pMapModel) delete pMapModel;
	if (pfenceModel) delete pfenceModel;
	
	//===============================//

	m_nEnemy = 3;
	m_ppEnemy = new CGameObject * [m_nEnemy];

	for (int i = 0; i < m_nEnemy; i++)
	{
		// 0 - Robot
		CLoadedModelInfo* pRobotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Robot.bin", NULL);
		m_ppEnemy[i] = new CRobotObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pRobotModel, 3);
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
	m_pMyPlayer->SetPosition(XMFLOAT3( 30.f, 30.f, 30.f));
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
	if (pKeysBuffer['6'] & 0xF0) {
#ifdef USE_NETWORK
		CS_TEST_PACKET p;
		p.size = sizeof(p);
		p.type = CS_TEST;
		p.x = 0.f;
		send_packet(&p);
#endif // USE_NETWORK

	}

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
			m_pMyPlayer->Move(dwDirection1, m_pMyPlayer->GetVelocitySpeed(), true);
	}

	m_dwLastDirection = dwDirection1;

	return(false);
}

bool PlayScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	CScene::OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);

	switch (nMessageID)
	{
	case WM_KEYDOWN: {
		switch (wParam) {
		case VK_SHIFT: {
			if (m_pMyPlayer->GetStaminer())m_pMyPlayer->SetRun(true);
			break;
		}
		case 'C': {
			if (m_pMyPlayer->GetStaminer())m_pMyPlayer->SetCreep();
			break;
		}
		}
		break;
	}

	case WM_KEYUP: {
		switch (wParam) {
		case VK_SHIFT: {
			m_pMyPlayer->SetRun(false);
			break;
		}
		case 'C': {
			m_pMyPlayer->SetCreepFlag();
			break;
		}
		}
		break;
	}
	default:
		break;
	}
	return false;
}

void PlayScene::ProcessPacket(char* p)
{
	switch (p[1])
	{
	case SC_ADD_PLAYER:
	{
		SC_ADD_PLAYER_PACKET* packet = reinterpret_cast<SC_ADD_PLAYER_PACKET*>(p);
		// cout << packet->id << " ADD" << endl;
		m_ppPlayer[packet->id]->p_id = packet->id;
		m_ppPlayer[packet->id]->m_bUnable = true;
		m_ppPlayer[packet->id]->SetPosition(packet->position);
		m_ppPlayer[packet->id]->Rotate(0.f, packet->rotation.y - m_ppPlayer[packet->id]->GetYaw(), 0.f);
	} break;

	case SC_MOVE_OBJECT:
	{
		SC_MOVE_OBJECT_PACKET* packet = reinterpret_cast<SC_MOVE_OBJECT_PACKET*>(p);
		// cout << packet->id << "Move" << endl;
		if (packet->id == my_id) break;
		else {
			m_ppPlayer[packet->id]->Move(packet->dir, true);
			// m_ppPlayer[packet->id]->SetPosition(packet->position);
			m_ppPlayer[packet->id]->Rotate(0.f, packet->yaw - m_ppPlayer[packet->id]->GetYaw(), 0.f);
		}

	} break;

	case SC_UPDATE_PLAYER:
	{
		SC_UPDATE_PLAYER_PACKET* packet = reinterpret_cast<SC_UPDATE_PLAYER_PACKET*>(p);
		if (packet->id == my_id) break;
		else {
			// m_ppPlayer[packet->id]->SetPosition(packet->position);

		}
	} break;

	case SC_CHANGE_ANIM: {
		SC_CHANGE_ANIMATION_PACKET* packet = reinterpret_cast<SC_CHANGE_ANIMATION_PACKET*>(p);
		if (packet->id == my_id) break;
		else {
			reinterpret_cast<CyborgPlayer*>(m_ppPlayer[packet->id])->m_pSkinnedAnimationController->m_fBlendingTime = 0.0f;
			reinterpret_cast<CyborgPlayer*>(m_ppPlayer[packet->id])->m_pasNextAni = packet->ani_st;

		}
	} break;

	case SC_ADD_NPC: {
		SC_ADD_NPC_PACKET* packet = reinterpret_cast<SC_ADD_NPC_PACKET*>(p);
		break;
		int n_id = packet->id - 100;
		m_ppEnemy[n_id]->SetPosition(packet->position);
	}
				   break;


	default:
		printf("Unknown PACKET type [%d]\n", p[1]);
	}
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
	
	if (pKeysBuffer['9'] & 0xF0) m_pMyPlayer->m_bUnable = true;
	if (pKeysBuffer['0'] & 0xF0) m_pMyPlayer->m_bUnable = false;

	// change Character
	if (pKeysBuffer['1'] & 0xF0 || pKeysBuffer['2'] & 0xF0 || pKeysBuffer['3'] & 0xF0)
	{
		Player_Character_Type select{ Robot };
		m_pMyPlayer->m_bUnable = false;
		if (pKeysBuffer['1'] & 0xF0)select = Corzim;
		if (pKeysBuffer['2'] & 0xF0)select = Evan;
		if (pKeysBuffer['3'] & 0xF0)select = Uranya;

		m_pMyPlayer = m_ppPlayer[select];

		m_pMyPlayer->m_bUnable = true;

#ifdef USE_NETWORK
		CS_CHANGE_CHARACTER_PACKET p;
		p.size = sizeof(p);
		p.type = CS_CHANGE_CHARACTER;
		p.c_type = select;
		send_packet(&p);
#endif // USE_NETWORK
	}
	



	if (pKeysBuffer['R'] & 0xF0) {
		m_pMyPlayer->m_bReady = !m_pMyPlayer->m_bReady;
#ifdef USE_NETWORK
		CS_GAMESTART_PACKET p;
		p.size = sizeof(p);
		p.type = CS_GAME_START;
		send_packet(&p);
#endif // USE_NETWORK
	}

	return(false);
}

bool CPrepareRoomScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	CScene::OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
	return false;
}

bool CPrepareRoomScene::AllPlayerReady()
{
	if (m_pMyPlayer->m_bReady) return true;
	else return false;
}

void CPrepareRoomScene::ProcessPacket(char* p)
{
	switch (p[1])
	{
	case SC_LOGIN_INFO:
	{
		SC_LOGIN_INFO_PACKET* packet = reinterpret_cast<SC_LOGIN_INFO_PACKET*>(p);
		my_id = m_ppPlayer[packet->id]->p_id = packet->id;
		cout << "My ID is " << my_id << " !" << endl;
		m_pMyPlayer = m_ppPlayer[my_id];
		m_pMyPlayer->m_bUnable = true;
	} break;

	case SC_CHANGE_CHARACTER: {
		SC_CHANGE_CHARACTER_PACKET* packet = reinterpret_cast<SC_CHANGE_CHARACTER_PACKET*>(p);

		if (packet->id == my_id) {

		}
		else {

		}

		cout << packet->id << ", " << packet->c_type << endl;

	}break;


	default:
		printf("Unknown PACKET type [%d]\n", p[1]);
	}
}



