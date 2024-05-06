
#include "stdafx.h"
#include "PlayScene.h"

void CPlayScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int myPlayernum)
{
	CScene::BuildObjects(pd3dDevice, pd3dCommandList, myPlayernum);

#ifdef USE_NETWORK
	CS_GAMESTART_PACKET p;
	p.size = sizeof(p);
	p.type = CS_GAME_START;
	send_packet(&p);
#endif // USE_NETWORK

	m_pUI = new CPlaySceneUI();

	//===============================//
	// SKY BOX (1)
	m_pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, L"SkyBox/SkyBox_sunset.dds");

	//===============================//
	// TERRAIN (1)
	XMFLOAT3 xmf3Scale(15.0f, 1.0f, 15.0f);
	XMFLOAT4 xmf4Color(0.2f, 0.2f, 0.2f, 0.0f);
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("Terrain/BaseTerrain.raw"), 257, 257, xmf3Scale, xmf4Color);
	m_pTerrain->SetPosition(XMFLOAT3(-2000.f, 0.f, -2000.f));

	//===============================//
	// Map (4)
	// [Present Setting]
	// 1 - 울타리				|| OBJ
	// 2 - 맵 1(우상단)			|| OBJ
	// 3 - 맵 2(좌하단)			|| OBJ

	// 4 - 점령 미션용 obj		|| OBJ



	m_nHierarchicalGameObjects = 3;
	m_ppHierarchicalGameObjects = new CGameObject * [m_nHierarchicalGameObjects];

	// 1 - obj1
	CLoadedModelInfo* pfenceModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/map/middle/Fence.bin", NULL);
	m_ppHierarchicalGameObjects[0] = new CStandardOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pfenceModel);
	if (pfenceModel) delete pfenceModel;

	CLoadedModelInfo* pMapModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/map/middle/MAP3_1.bin", NULL);
	m_ppHierarchicalGameObjects[1] = new CStandardOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pMapModel);
	if (pMapModel) delete pMapModel;
	
	CLoadedModelInfo* pMapModel2 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/map/middle/MAP3_2.bin", NULL);
	m_ppHierarchicalGameObjects[2] = new CStandardOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pMapModel2);
	if (pMapModel2) delete pMapModel2;


	//===============================//
	// Mission Obj(1)
	m_nMissionObj = 2;
	m_ppMissionObj = new CMissonOBJ * [m_nMissionObj];

	CLoadedModelInfo* pMachine = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/test/Comm.bin", NULL);
	XMFLOAT3 missionRange = XMFLOAT3(30.f, 30.f, 30.f);
	m_ppMissionObj[0] = new CMissonOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pMachine, missionRange);
	m_ppMissionObj[0]->SetPosition(300.f, 0.f, 700.f);
	if (pMachine) delete pMachine;

	CLoadedModelInfo* pMachine2 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/test/Comm.bin", NULL);
	m_ppMissionObj[1] = new CMissonOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pMachine2, missionRange);
	m_ppMissionObj[1]->SetPosition(100.f, 0.f, 700.f);
	if (pMachine2) delete pMachine2;

	//===============================//

	m_nEnemy = 3;
	m_ppEnemy = new CGameObject * [m_nEnemy];

	for (int i = 0; i < m_nEnemy; i++)
	{
		// 0 - Robot
		CLoadedModelInfo* pRobotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Robot.bin", NULL);
		m_ppEnemy[i] = new CRobotObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pRobotModel, 3);
		m_ppEnemy[i]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
		m_ppEnemy[i]->SetPosition(NPCInitPos[i]);
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

	int playernum = 0;
	if (myPlayernum != 4) playernum = myPlayernum;

	m_pMyPlayer = m_ppPlayer[playernum];
	// m_pMyPlayer->SetPosition(XMFLOAT3(50.f, 0.f, 70.f));
	m_pMyPlayer->SetPosition(PlayerInitPos[playernum]);


}


bool CPlayScene::ProcessInput(HWND m_hWnd, POINT m_ptOldCursorPos, UCHAR* pKeysBuffer)
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
	if (pKeysBuffer['7'] & 0xF0) m_pMyPlayer->m_bUnable = true;

	// Player disable
	if (pKeysBuffer['4'] & 0xF0) m_ppPlayer[FIRST_PLAYER]->m_bUnable = false;
	if (pKeysBuffer['5'] & 0xF0) m_ppPlayer[SECOND_PLAYER]->m_bUnable = false;

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


		if (dwDirection1 && m_pMyPlayer->m_bUnable) {
			m_pMyPlayer->Move(dwDirection1, m_pMyPlayer->GetVelocitySpeed(), true);

		}
	}

	m_dwLastDirection = dwDirection1;

	return(false);
}

bool CPlayScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
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
			// m_pMyPlayer->SetCreepFlag();
			
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

void CPlayScene::AnimateObjects(float fTimeElapsed)
{
	CScene::AnimateObjects(fTimeElapsed);

	if (m_pUI->MissionGauge < 170)
	{
		if (Missionflag) m_pUI->MissionGauge += 0.5f;
		else if (m_pUI->MissionGauge > 0) m_pUI->MissionGauge -= 0.5f;
	}

}


void CPlayScene::ProcessPacket(char* p)
{
	switch (p[1])
	{
	case SC_ADD_PLAYER:
	{
		SC_ADD_PLAYER_PACKET* packet = reinterpret_cast<SC_ADD_PLAYER_PACKET*>(p);
		// m_ppPlayerSelecter[c_id]
		Player_Character_Type type = packet->c_type;

		idANDtype.insert({ packet->id, packet->c_type });

		// m_pMyPlayer = m_ppPlayer[packet->c_type];

		m_ppPlayer[type]->p_id = packet->id;
		m_ppPlayer[type]->m_bUnable = true;
		m_ppPlayer[type]->SetPosition(packet->position);
		m_ppPlayer[type]->Rotate(0.f, packet->rotation.y - m_ppPlayer[type]->GetYaw(), 0.f);
	} break;

	case SC_MOVE_OBJECT:
	{
		SC_MOVE_OBJECT_PACKET* packet = reinterpret_cast<SC_MOVE_OBJECT_PACKET*>(p);
		int id = packet->id;
		if (id == my_id) break;


		auto it = idANDtype.find(id);
		if (it == idANDtype.end()) break;
		else {
			Player_Character_Type type = it->second;
			m_ppPlayer[type]->Move(packet->dir, true);
			m_ppPlayer[type]->Rotate(0.f, packet->yaw - m_ppPlayer[type]->GetYaw(), 0.f);

		}

	} break;

	case SC_UPDATE_PLAYER:
	{
		SC_UPDATE_PLAYER_PACKET* packet = reinterpret_cast<SC_UPDATE_PLAYER_PACKET*>(p);
		int id = packet->id;
		if (id == my_id) break;


		auto it = idANDtype.find(id);
		if (it == idANDtype.end()) break;
		else {
			Player_Character_Type type = it->second;
			m_ppPlayer[type]->SetPosition(packet->position);
			cout << "[" << type << "]" << endl;
			// m_ppPlayer[type]->Rotate(0.f, packet->yaw - m_ppPlayer[type]->GetYaw(), 0.f);

		}

	} break;

	case SC_CHANGE_ANIM: {
		SC_CHANGE_ANIMATION_PACKET* packet = reinterpret_cast<SC_CHANGE_ANIMATION_PACKET*>(p);
		int id = packet->id;
		auto& it = idANDtype.find(id);
		if (id == my_id) break;

		if (it == idANDtype.end()) break;
		else {


			Player_Character_Type type = it->second;
			reinterpret_cast<CyborgPlayer*>(m_ppPlayer[type])->m_pSkinnedAnimationController->m_fBlendingTime = 0.0f;
			reinterpret_cast<CyborgPlayer*>(m_ppPlayer[type])->m_pasNextAni = packet->ani_st;

		}

	} break;

	case SC_ADD_NPC: {
		SC_ADD_NPC_PACKET* packet = reinterpret_cast<SC_ADD_NPC_PACKET*>(p);
		int n_id = packet->id - 100;
		m_ppEnemy[n_id]->SetPosition(packet->position);
		// reinterpret_cast<CRobotObject*>(m_ppEnemy[n_id])->SetTarget(m_ppEnemy[n_id]->GetPosition());

	}
				   break;

	case SC_MOVE_NPC: {
		SC_MOVE_NPC_PACKET* packet = reinterpret_cast<SC_MOVE_NPC_PACKET*>(p);

		int n_id = packet->id - 100;
		reinterpret_cast<CRobotObject*>(m_ppEnemy[n_id])->SetTarget(packet->next_pos);
		// std::cout << m_ppEnemy[n_id]->GetPosition().x << "," << m_ppEnemy[n_id]->GetPosition().z << std::endl;
		std::cout << packet->next_pos.x << "," << packet->next_pos.z << std::endl;
	}
					break;

	case SC_ATTACK_NPC: {
		SC_ATTACK_NPC_PACKET* packet = reinterpret_cast<SC_ATTACK_NPC_PACKET*>(p);
		cout << "[" << packet->p_id << "] 사망" << endl;
	}break;


	default:
		printf("Scene[Stage1] - Unknown PACKET type [%d]\n", p[1]);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
void CPrepareRoomScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int myPlayernum)
{
	CScene::BuildObjects(pd3dDevice, pd3dCommandList, myPlayernum);

	m_pUI = new CFirstSceneUI();

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	//===============================//
	// SKY BOX (1)
	m_pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, L"SkyBox/SkyBox_0.dds");


	//===============================//
	// TERRAIN
	XMFLOAT3 xmf3Scale(10.f, 10.f, 10.f);
	XMFLOAT4 xmf4Color(0.0f, 0.0f, 0.0f, 0.0f);
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("Terrain/BaseTerrain.raw"), 257, 257, xmf3Scale, xmf4Color);
	m_pTerrain->SetPosition(-1000.f, 0, -1000.f);

	//===============================//
	// OBG

	m_nHierarchicalGameObjects = 3;
	m_ppHierarchicalGameObjects = new CGameObject * [m_nHierarchicalGameObjects];

	CLoadedModelInfo* pContainerModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/ObjModel/ConcreteWall.bin", NULL);
	m_ppHierarchicalGameObjects[0] = new CStandardOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pContainerModel);
	m_ppHierarchicalGameObjects[0]->SetScale(8.f, 4.f, 4.f);
	m_ppHierarchicalGameObjects[0]->Rotate(0.f, 90.f, 0.f);
	m_ppHierarchicalGameObjects[0]->SetPosition(-60.f, 0.f, 40.f);
	if (pContainerModel) delete pContainerModel;


	CLoadedModelInfo* pMiddleContainer = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/ObjModel/MiddleContainer.bin", NULL);
	m_ppHierarchicalGameObjects[1] = new CStandardOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pMiddleContainer);
	m_ppHierarchicalGameObjects[1]->SetPosition(-35.f, 0.f, 0.f);

	m_ppHierarchicalGameObjects[2] = new CStandardOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pMiddleContainer);
	m_ppHierarchicalGameObjects[2]->SetPosition(35.f, 0.f, 0.f);
	if (pMiddleContainer) delete pMiddleContainer;


	//===============================//
	// OBG

	m_nPlayerSelecter = 3;
	m_ppPlayerSelecter = new CSelectCharacterOBJ * [m_nPlayerSelecter];

	m_ppPlayerSelecter[0] = new CSelectCharacterOBJ(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), Robot, 3);
	m_ppPlayerSelecter[0]->Rotate(0.f, 180.f, 0.f);
	m_ppPlayerSelecter[0]->SetPosition(-15.0f, 0.0f, 0.0f);

	m_ppPlayerSelecter[1] = new CSelectCharacterOBJ(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), Robot, 3);
	m_ppPlayerSelecter[1]->Rotate(0.f, 180.f, 0.f);
	m_ppPlayerSelecter[1]->SetPosition(0.0f, 0.0f, 0.0f);

	m_ppPlayerSelecter[2] = new CSelectCharacterOBJ(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), Robot, 3);
	m_ppPlayerSelecter[2]->Rotate(0.f, 180.f, 0.f);
	m_ppPlayerSelecter[2]->SetPosition(15.0f, 0.0f, 0.0f);


	//===============================//
	// Player
	m_nPlayer = 1;
	m_ppPlayer = new CPlayer * [m_nPlayer];
	m_ppModelInfoPlayer = new CLoadedModelInfo * [m_nPlayer]; // Play Character

	// 저장된 모델 바꿀 수 있음
	m_ppModelInfoPlayer[FIRST_PLAYER] = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), "Model/Player_1.bin", NULL);

	for (int i = 0; i < m_nPlayer; ++i) {
		CyborgPlayer* pPlayer = new CyborgPlayer(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), m_pTerrain, m_ppModelInfoPlayer[i]);
		m_ppPlayer[i] = pPlayer;
		m_ppPlayer[i]->SetPlayerData(i);
	}

	m_pMyPlayer = m_ppPlayer[MY_PLAYER];
	m_pMyPlayer->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_pMyPlayer->ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);
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

	//if (pKeysBuffer['4'] & 0xF0) ChangeModel(0, Evan);

	// change Character
	//if (pKeysBuffer['1'] & 0xF0 || pKeysBuffer['2'] & 0xF0 || pKeysBuffer['3'] & 0xF0)
	//{
	//	Player_Character_Type select{ Robot };

	//	if (pKeysBuffer['1'] & 0xF0) select = Corzim;
	//	if (pKeysBuffer['2'] & 0xF0)select = Evan;
	//	if (pKeysBuffer['3'] & 0xF0)select = Uranya;


	//}

//	if (pKeysBuffer['R'] & 0xF0) {
//#ifdef USE_NETWORK
//		
//#endif // USE_NETWORK
//
//		// m_pMyPlayer->m_bReady = !m_pMyPlayer->m_bReady;
//	}

	return(false);
}

bool CPrepareRoomScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN: {
		switch (wParam) {

		case 'R': {
			CS_GAMESTART_PACKET p;
			p.size = sizeof(p);
			p.type = CS_ALLPLAYER_READY;
#ifdef USE_NETWORK
			send_packet(&p);
#else
			m_pMyPlayer->m_bReady = !m_pMyPlayer->m_bReady;
#endif // USE_NETWORK

		}break;
		case '1': {
			select = Corzim;

		}break;
		case '2': {
			select = Evan;
		}break;
		case '3': {
			select = Uranya;
		}break;

		}
	default:
		break;
	}
	}

#ifdef USE_NETWORK
	CS_CHANGE_CHARACTER_PACKET p;
	p.size = sizeof(p);
	p.type = CS_CHANGE_CHARACTER;
	p.c_type = select;
	send_packet(&p);
#else
	ChangeModel(0, select);
#endif // USE_NETWORK
	CScene::OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
	return false;
}

void CPrepareRoomScene::ReleaseObjects()
{
	if (m_ppPlayerSelecter)
	{
		for (int i = 0; i < m_nPlayerSelecter; i++)
		{
			m_ppPlayerSelecter[i]->ReleaseUploadBuffers();
			m_ppPlayerSelecter[i]->Release();

		}
		delete[] m_ppPlayerSelecter;
	}

	CScene::ReleaseObjects();
}

void CPrepareRoomScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	CScene::Render(pd3dCommandList, pCamera);

	for (int i = 0; i < m_nPlayerSelecter; i++)
	{
		if (m_ppPlayerSelecter[i])
		{
			if (!m_ppPlayerSelecter[i]->m_pSkinnedAnimationController) m_ppPlayerSelecter[i]->UpdateTransform(NULL);
			m_ppPlayerSelecter[i]->Animate(m_fElapsedTime);
			m_ppPlayerSelecter[i]->Render(pd3dCommandList, pCamera);
		}
	}
}

void CPrepareRoomScene::ReleaseUploadBuffers()
{
	CScene::ReleaseUploadBuffers();

	for (int i = 0; i < m_nPlayerSelecter; i++) m_ppPlayerSelecter[i]->ReleaseUploadBuffers();

}

bool CPrepareRoomScene::AllPlayerReady()
{
	if (m_pMyPlayer->m_bReady) return true;
	else return false;
}

void CPrepareRoomScene::SetChangedModel(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < m_nPlayerSelecter; i++)
	{
		if (m_ppPlayerSelecter[i]->m_bChanged)
		{
			XMFLOAT3 beforeLoc = m_ppPlayerSelecter[i]->GetPosition();

			CSelectCharacterOBJ* pTemp = new CSelectCharacterOBJ(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), m_ppPlayerSelecter[i]->m_nChangedModelNum, 1);

			if (m_ppPlayerSelecter[i])
				delete m_ppPlayerSelecter[i];

			m_ppPlayerSelecter[i] = pTemp;
			m_ppPlayerSelecter[i]->SetPosition(beforeLoc);
			m_ppPlayerSelecter[i]->Rotate(0.f, 180.f, 0.f);
			m_ppPlayerSelecter[i]->m_bChanged = false;
		}
	}
}

void CPrepareRoomScene::ChangeModel(int nPlayer, int nModel)
{
	m_ppPlayerSelecter[nPlayer]->m_nChangedModelNum = nModel;
	m_ppPlayerSelecter[nPlayer]->m_bChanged = true;
}


int CPrepareRoomScene::getModelInfo()
{
	int playerId = 0;
	if (my_id != -1) playerId = my_id;
	return m_ppPlayerSelecter[playerId]->m_nModelNum;
}


void CPrepareRoomScene::ProcessPacket(char* p)
{
	switch (p[1])
	{
	case SC_LOGIN_INFO:
	{
		SC_LOGIN_INFO_PACKET* packet = reinterpret_cast<SC_LOGIN_INFO_PACKET*>(p);
		my_id = packet->id;
		cout << "My ID is " << my_id << " !" << endl;
	} break;

	case SC_CHANGE_CHARACTER: {
		SC_CHANGE_CHARACTER_PACKET* packet = reinterpret_cast<SC_CHANGE_CHARACTER_PACKET*>(p);
		cout << packet->id << " -> " << packet->c_type << endl;
		ChangeModel(packet->id, packet->c_type);

	}break;

	case SC_GAME_START: {
		m_pMyPlayer->m_bReady = !m_pMyPlayer->m_bReady;
		// 플레이게임으로 씬전환
		break;
	}


	default:
		printf("Scene[LOBBY] - Unknown PACKET type [%d]\n", p[1]);
		break;
	}
}



