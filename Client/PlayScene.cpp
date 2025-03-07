
#include "stdafx.h"
#include "PlayScene.h"

void CFirstRoundScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int myPlayernum)
{
	CScene::BuildObjects(pd3dDevice, pd3dCommandList, myPlayernum);

#ifdef USE_NETWORK
	CS_GAMESTART_PACKET p;
	p.size = sizeof(p);
	p.type = CS_GAME_START;
	send_packet(&p);
#endif // USE_NETWORK

	m_pUI = new CFirstRoundSceneUI();

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

	m_nHierarchicalGameObjects = 5;
	m_ppHierarchicalGameObjects = new CGameObject * [m_nHierarchicalGameObjects];

	// 1 - obj1
	CLoadedModelInfo* pfenceModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/map/Fence.bin", NULL);
	m_ppHierarchicalGameObjects[0] = new CStandardOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pfenceModel);
	if (pfenceModel) delete pfenceModel;

	CLoadedModelInfo* pMapModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/map/MAP1_Modified.bin", NULL);
	m_ppHierarchicalGameObjects[1] = new CStandardOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pMapModel);
	if (pMapModel) delete pMapModel;

	CLoadedModelInfo* pMapModel2 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/map/MAP2_Modified.bin", NULL);
	m_ppHierarchicalGameObjects[2] = new CStandardOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pMapModel2);
	if (pMapModel2) delete pMapModel2;


	CLoadedModelInfo* pMapModel3 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/map/Section1_modify.bin", NULL);
	m_ppHierarchicalGameObjects[3] = new CStandardOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pMapModel3);
	if (pMapModel3) delete pMapModel3;

	CLoadedModelInfo* pMapModel4 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/map/First_section.bin", NULL);
	m_ppHierarchicalGameObjects[4] = new CStandardOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pMapModel4);
	if (pMapModel4) delete pMapModel4;


	m_nFloorObj = 19;
	
	m_ppFloorObj = new CGameObject * [m_nFloorObj];

	CLoadedModelInfo* pFloormodel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/ObjModel/Mission/Occ_Range.bin", NULL);
	m_ppFloorObj[0] = new CFloorObj(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pFloormodel);
	m_ppFloorObj[0]->SetPosition(100.f, 0.3f, 930.f);

	m_ppFloorObj[1] = new CFloorObj(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pFloormodel);
	m_ppFloorObj[1]->SetPosition(650.f, 0.3f, 100.f);

	m_ppFloorObj[2] = new CFloorObj(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pFloormodel);
	m_ppFloorObj[2]->SetPosition(500.f, 0.3f, 500.f);
	if (pFloormodel) delete pFloormodel;

	CLoadedModelInfo* pMainFloormodel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/ObjModel/StandardFloor.bin", NULL);

	for (int i = 0; i < 4; i++)
	{

		m_ppFloorObj[3 + i * 4] = new CFloorObj(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pMainFloormodel);
		m_ppFloorObj[3 + i * 4]->SetScale(3.f, 0.f, 3.f);
		m_ppFloorObj[3 + i * 4]->SetPosition(-200.f, 0.3f, i * 400 - 200.f);

		m_ppFloorObj[4 + i * 4] = new CFloorObj(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pMainFloormodel);
		m_ppFloorObj[4 + i * 4]->SetScale(3.f, 0.f, 3.f);
		m_ppFloorObj[4 + i * 4]->SetPosition(200.f, 0.3f, i * 400 - 200.f);

		m_ppFloorObj[5 + i * 4] = new CFloorObj(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pMainFloormodel);
		m_ppFloorObj[5 + i * 4]->SetScale(3.f, 0.f, 3.f);
		m_ppFloorObj[5 + i * 4]->SetPosition(600.f, 0.3f, i * 400 - 200.f);

		m_ppFloorObj[6 + i * 4] = new CFloorObj(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pMainFloormodel);
		m_ppFloorObj[6 + i * 4]->SetScale(3.f, 0.f, 3.f);
		m_ppFloorObj[6 + i * 4]->SetPosition(1000.f, 0.3f, i * 400 - 200.f);

	}

	if (pMainFloormodel) delete pMainFloormodel;
	

	//===============================//
	// Mission Obj(1)
	m_nMissionObj = 6;
	m_ppMissionObj = new CMissonOBJ * [m_nMissionObj];

	// 미션 범위
	XMFLOAT3 OccMissionRange = XMFLOAT3(37.f, 30.f, 37.f);
	XMFLOAT3 MissionRange = XMFLOAT3(10.f, 20.f, 10.f);

	// 점령미션
	CLoadedModelInfo* pMachine = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/ObjModel/Mission/Occ_Machine.bin", NULL);
	m_ppMissionObj[0] = new CMissonOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pMachine, OccMissionRange, 0);
	m_ppMissionObj[0]->SetPosition(100.f, 0.0f, 930.f);
	if (pMachine) delete pMachine;

	CLoadedModelInfo* pMachine2 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/ObjModel/Mission/Occ_Machine.bin", NULL);
	m_ppMissionObj[1] = new CMissonOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pMachine2, OccMissionRange, 0);
	m_ppMissionObj[1]->SetPosition(650.f, 0.f, 100.f);
	if (pMachine2) delete pMachine2;

	CLoadedModelInfo* pMachine3 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/ObjModel/Mission/Occ_Machine.bin", NULL);
	m_ppMissionObj[2] = new CMissonOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pMachine3, OccMissionRange, 0);
	m_ppMissionObj[2]->SetPosition(500.f, 0.f, 500.f);
	if (pMachine3) delete pMachine3;

	// 보안키 미션
	CLoadedModelInfo* pMssionMachine1 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/ObjModel/Mission/MissionMachine_2.bin", NULL);
	m_ppMissionObj[3] = new CMissonOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pMssionMachine1, MissionRange, 1);
	m_ppMissionObj[3]->Rotate(0.f, 0.f, 0.f);
	m_ppMissionObj[3]->SetPosition(525.f, -5.f, 260.f);
	if (pMssionMachine1) delete pMssionMachine1;

	CLoadedModelInfo* pMssionMachine2 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/ObjModel/Mission/MissionMachine_2.bin", NULL);
	m_ppMissionObj[4] = new CMissonOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pMssionMachine2, MissionRange, 1);
	m_ppMissionObj[4]->Rotate(0.f, -90.f, 0.f);
	m_ppMissionObj[4]->SetPosition(780.f, -5.f, 750.f);
	if (pMssionMachine2) delete pMssionMachine2;

	CLoadedModelInfo* pMssionMachine3 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/ObjModel/Mission/MissionMachine_2.bin", NULL);
	m_ppMissionObj[5] = new CMissonOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pMssionMachine3, MissionRange, 1);
	m_ppMissionObj[5]->Rotate(0.f, -90.f, 0.f);
	m_ppMissionObj[5]->SetPosition(133.f, -5.f, 200.f);
	if (pMssionMachine3) delete pMssionMachine3;


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
		//CyborgPlayer* pPlayer = new CyborgPlayer(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), m_pTerrain, m_ppModelInfoPlayer[i], THIRD_PERSON_CAMERA);
		m_ppPlayer[i] = new CyborgPlayer(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), m_pTerrain, m_ppModelInfoPlayer[i], THIRD_PERSON_CAMERA);
		m_ppPlayer[i]->SetPlayerData(i);
	}

	int playernum = 0;
	if (myPlayernum != 4) playernum = myPlayernum;

	m_pMyPlayer = m_ppPlayer[playernum];
	// m_pMyPlayer->SetPosition(XMFLOAT3(50.f, 0.f, 70.f));
	m_pMyPlayer->SetPosition(PlayerInitPos[playernum]);
	m_pMyPlayer->ChangeCamera(SHOULDER_VIEW_CAMERA, 0.0f);

	reinterpret_cast<CFirstRoundSceneUI*>(m_pUI)->m_fMaxStamina = reinterpret_cast<CyborgPlayer*>(m_pMyPlayer)->m_fMaxStaminer;

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

}

void CFirstRoundScene::ReleaseObjects()
{
	if (m_ppFloorObj)
	{
		for (int i = 0; i < m_nFloorObj; i++) if (m_ppFloorObj[i])
		{
			m_ppFloorObj[i]->ReleaseUploadBuffers();
			m_ppFloorObj[i]->Release();
		}
		delete[] m_ppFloorObj;
	}
	CScene::ReleaseObjects();
}

void CFirstRoundScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	CScene::Render(pd3dCommandList, pCamera);
	for (int i = 0; i < m_nFloorObj; i++)
	{
		if (m_ppFloorObj[i])
		{
			if (!m_ppFloorObj[i]->m_pSkinnedAnimationController) m_ppFloorObj[i]->UpdateTransform(NULL);
			m_ppFloorObj[i]->Animate(m_fElapsedTime);
			m_ppFloorObj[i]->Render(pd3dCommandList, pCamera);
		}
	}

}

void CFirstRoundScene::ReleaseUploadBuffers()
{
	CScene::ReleaseUploadBuffers();
	for (int i = 0; i < m_nFloorObj; i++) m_ppFloorObj[i]->ReleaseUploadBuffers();

}


bool CFirstRoundScene::ProcessInput(HWND m_hWnd, POINT m_ptOldCursorPos, UCHAR* pKeysBuffer)
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

	if (pKeysBuffer['7'] & 0xF0) m_pMyPlayer->m_bUnable = true;

	// Decide whether to blend
	// 전에 입력한 키와 다르다면 블렌딩타임을 0으로 설정
	// 블렌딩 타임이 0 -> 블렌딩 시작
	// 블렌딩 타임이 1 -> 블렌딩 완료

	//if (m_dwLastDirection != dwDirection1) {
	//	m_pMyPlayer->m_pSkinnedAnimationController->m_fBlendingTime = 0.0f;
	//}

	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f) || (dwDirection1 != 0))
	{
		if ((cxDelta || cyDelta) && m_pMyPlayer->m_bUnable) {
			m_pMyPlayer->CameraRotate(0.0f, cxDelta, 0.0f);
			m_pMyPlayer->m_pCamera->RotatePitch(-cyDelta / 2.4);
		}


		if (dwDirection1 && m_pMyPlayer->m_bUnable) {
			reinterpret_cast<CyborgPlayer*>(m_pMyPlayer)->StartKeyMission(-1);
			if (!reinterpret_cast<CyborgPlayer*>(m_pMyPlayer)->m_bIsCrawl)
				m_pMyPlayer->Move(dwDirection1, m_pMyPlayer->GetVelocitySpeed(), true);
		}
	}

	m_dwLastDirection = dwDirection1;

	return(false);
}

bool CFirstRoundScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	CScene::OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);

	switch (nMessageID)
	{

		//============ KeyDown ============//
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
		case 'L': {
			CS_ALIVE_PLAYER_PACKET p;
			p.size = sizeof(p);
			p.type = CS_ALIVE_PLAYER;
			p.id = my_id;
			send_packet(&p);
			break;
		}

		case 'K':
		{
			reinterpret_cast<CRobotObject*>(m_ppEnemy[0])->SetAttackStatus(true);
			reinterpret_cast<CRobotObject*>(m_ppEnemy[0])->SetTarget(m_pMyPlayer->GetPosition());
			reinterpret_cast<CRobotObject*>(m_ppEnemy[1])->SetAttackStatus(true);
			reinterpret_cast<CRobotObject*>(m_ppEnemy[1])->SetTarget(m_pMyPlayer->GetPosition());
			reinterpret_cast<CRobotObject*>(m_ppEnemy[2])->SetAttackStatus(true);
			reinterpret_cast<CRobotObject*>(m_ppEnemy[2])->SetTarget(m_pMyPlayer->GetPosition());
			break;
		}
		case 'F': {
			if (!reinterpret_cast<CyborgPlayer*>(m_pMyPlayer)->m_bIsCrawl) {
				Player_Interaction_Type InteractionType = CheckInteraction();
				Interaction(InteractionType);
			}
			break;
		}
		case '1':
		case '2':
		case '3':
			reinterpret_cast<CyborgPlayer*>(m_pMyPlayer)->MissionCheck(wParam - 49);
			break;

		case '9':
			Send_Go_Stage2();
			break;
		}
		break;
	}

				   //============ KeyUP ============//

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
		case '9': { // 스테이지 2로 변경

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

int CFirstRoundScene::IsCrawlPlayer()
{
	for (int i = 0; i < m_nPlayer; ++i) {
		if (m_ppPlayer[i]->m_xmBoundingBox.Intersects(m_pMyPlayer->m_xmBoundingBox) && reinterpret_cast<CyborgPlayer*>(m_pMyPlayer)->GetCrawl()) {
			CS_ALIVE_PLAYER_PACKET p;
			p.size = sizeof(p);
			p.type = CS_ALIVE_PLAYER;
			p.id = my_id;
			send_packet(&p);
		}
	}
	return 0;
}

bool CFirstRoundScene::CheckMissionObj()
{
	for (int i = 3; i < m_nMissionObj; i++)
	{
		if (m_ppMissionObj[i]->m_bMissionflag)
		{
			if (!reinterpret_cast<CyborgPlayer*>(m_pMyPlayer)->GetSecurityKey())
				return true;
		}
	}
	return false;
}

bool CFirstRoundScene::CheckHeal()
{
	for (int i = 0; i < m_nPlayer; ++i) {
		if (m_pMyPlayer != m_ppPlayer[i] && m_pMyPlayer->m_xmBoundingBox.Intersects(m_ppPlayer[i]->m_xmBoundingBox)) {
			if (reinterpret_cast<CyborgPlayer*>(m_ppPlayer[i])->GetCurrentAni() == CRAWL) {
				reinterpret_cast<CyborgPlayer*>(m_pMyPlayer)->SetHealTarget(m_ppPlayer[i]->p_id);
				return true;
			}
		}
	}

	return false;
}

Player_Interaction_Type CFirstRoundScene::CheckInteraction()
{
	if (CheckHeal())return Heal;
	if (CheckMissionObj())return CardMission;
	return NON;
}

void CFirstRoundScene::Interaction(Player_Interaction_Type type)
{
	switch (type) {
	case Heal: {
		int id = reinterpret_cast<CyborgPlayer*>(m_pMyPlayer)->GetHealTarget();
		cout << "<Heal>" << my_id << "->" << id << endl;
		CS_ALIVE_PLAYER_PACKET p;
		p.size = sizeof(p);
		p.type = CS_ALIVE_PLAYER;
		p.id = id;
		send_packet(&p);
		break;
	}
	case CardMission: {
		reinterpret_cast<CyborgPlayer*>(m_pMyPlayer)->StartKeyMission(0);
		break;
	}
	case FinalMission: {
		break;
	}
	}
}

void CFirstRoundScene::AnimateObjects(float fTimeElapsed)
{
	CScene::AnimateObjects(fTimeElapsed);

	for (int i = 0; i < m_nMissionObj; i++)
	{
		Missionflag = false;
		if (m_ppMissionObj[i]->m_nCategory == 0) {
			for (int j = 0; j < m_nPlayer; j++) if (m_ppPlayer[j]) {
				if (CheckMissionBound(m_ppPlayer[j], m_ppMissionObj[i]) && reinterpret_cast<CyborgPlayer*>(m_ppPlayer[j])->GetSecurityKey())
				{
					Missionflag = true;
				}
			}
		}
		else {
			for (int j = 0; j < m_nPlayer; j++) if (m_ppPlayer[j]) {
				if (CheckMissionBound(m_ppPlayer[j], m_ppMissionObj[i]))
				{
					Missionflag = true;
				}
			}
		}


		m_ppMissionObj[i]->m_bMissionflag = Missionflag;

	}

	for (int i = 0; i < m_nMissionObj; i++)
	{
		if (m_ppMissionObj[i]->m_nCategory == 0)	// 점령미션
		{
			if (m_pUI->m_fMissionGauge[i] < 370)
			{
				if (m_ppMissionObj[i]->m_bMissionflag)
				{
					m_pUI->m_fMissionGauge[i] += 0.5f;
				}
				else if (m_pUI->m_fMissionGauge[i] > 0) m_pUI->m_fMissionGauge[i] -= 1.0f;
			}
			else
			{
				m_ppMissionObj[i]->m_bEnd = true;
			}
		}
		else
		{
			break;
		}

	}

	m_pUI->m_bcard = reinterpret_cast<CyborgPlayer*>(m_pMyPlayer)->GetSecurityKey();

	if (reinterpret_cast<CyborgPlayer*>(m_pMyPlayer)->m_fStaminer < reinterpret_cast<CyborgPlayer*>(m_pMyPlayer)->m_fMaxStaminer)
	{
		reinterpret_cast<CFirstRoundSceneUI*>(m_pUI)->m_bStaminaBarOn = true;
		reinterpret_cast<CFirstRoundSceneUI*>(m_pUI)->m_fStaminaRange = reinterpret_cast<CyborgPlayer*>(m_pMyPlayer)->m_fStaminer;
	}
	else reinterpret_cast<CFirstRoundSceneUI*>(m_pUI)->m_bStaminaBarOn = false;

	int endcheck = 0;

	for (int i = 0; i < m_nMissionObj; i++)
	{
		if (m_ppMissionObj[i]->m_bEnd) endcheck += 1;
	}

	if (endcheck == 3)
	{
		Send_Go_Stage2();
	}

}


int CFirstRoundScene::FindID(Player_Character_Type type)
{
	for (int i = 0; i < 3; ++i)
		if (idANDtype.find(i)->second == type)
			return i;
	return -1;
}

void CFirstRoundScene::ProcessPacket(char* p)
{
	switch (p[1])
	{
	case SC_ADD_PLAYER:
	{
		SC_ADD_PLAYER_PACKET* packet = reinterpret_cast<SC_ADD_PLAYER_PACKET*>(p);
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
	}
				   break;

	case SC_MOVE_NPC: {
		SC_MOVE_NPC_PACKET* packet = reinterpret_cast<SC_MOVE_NPC_PACKET*>(p);

		int n_id = packet->id - 100;
		// reinterpret_cast<CRobotObject*>(m_ppEnemy[n_id])->SetPosition(reinterpret_cast<CRobotObject*>(m_ppEnemy[n_id])->GetTarget());
		reinterpret_cast<CRobotObject*>(m_ppEnemy[n_id])->SetTarget(packet->next_pos);
		// std::cout << m_ppEnemy[n_id]->GetPosition().x << "," << m_ppEnemy[n_id]->GetPosition().z << std::endl;
		// std::cout << packet->next_pos.x << "," << packet->next_pos.z << std::endl;
	}
					break;

	case SC_ATTACK_NPC: {
		SC_ATTACK_NPC_PACKET* packet = reinterpret_cast<SC_ATTACK_NPC_PACKET*>(p);
		//------------------------
		int id = packet->p_id;
		auto& it = idANDtype.find(id);
		XMFLOAT3 xmf3{};
		if (it == idANDtype.end()) break;
		else {
			Player_Character_Type type = it->second;
			xmf3 = reinterpret_cast<CyborgPlayer*>(m_ppPlayer[type])->GetPosition();
		}
		//------------------------
		int n_id = packet->n_id - 100;
		//
		reinterpret_cast<CRobotObject*>(m_ppEnemy[n_id])->SetAttackStatus(true);
		reinterpret_cast<CRobotObject*>(m_ppEnemy[n_id])->SetTarget(xmf3);
		// cout << "공격" << m_ppEnemy[n_id]->GetPosition().x << "," << m_ppEnemy[n_id]->GetPosition().z << endl;

		CS_NPC_UPDATE_PACKET nup;
		nup.size = sizeof(nup);
		nup.type = CS_NPC_UPDATE;
		nup.n_id = packet->n_id;
		nup.position = m_ppEnemy[n_id]->GetPosition();
		send_packet(&nup);


	}break;

	case SC_NPC_UPDATE: {
		SC_NPC_UPDATE_PACKET* packet = reinterpret_cast<SC_NPC_UPDATE_PACKET*>(p);
		int n_id = packet->n_id - 100;
		m_ppEnemy[n_id]->SetPosition(packet->position);
	}break;

	case SC_GETKEY: {
		SC_GETKEY_PACKET* packet = reinterpret_cast<SC_GETKEY_PACKET*>(p);

		int id = packet->p_id;
		auto& it = idANDtype.find(id);

		if (it == idANDtype.end()) break;
		else {

			Player_Character_Type type = it->second;
			reinterpret_cast<CyborgPlayer*>(m_ppPlayer[type])->ChangeKeyState(true);
		}

	}break;

	case SC_PLAYER_DEATH: {
		SC_PLAYER_DEATH_PACKET* packet = reinterpret_cast<SC_PLAYER_DEATH_PACKET*>(p);

		int id = packet->id;
		auto& it = idANDtype.find(id);
		if (it == idANDtype.end()) break;
		else {

			Player_Character_Type type = it->second;
			reinterpret_cast<CyborgPlayer*>(m_ppPlayer[type])->SetCrawl(true);
			cout << "사망" << m_ppEnemy[0]->GetPosition().x << "," << m_ppEnemy[0]->GetPosition().z << endl;
		}
		if (packet->id == my_id)
			reinterpret_cast<CyborgPlayer*>(m_pMyPlayer)->StartKeyMission(-1);

	}break;

	case SC_PLAYER_ALIVE: {
		SC_PLAYER_ALIVE_PACKET* packet = reinterpret_cast<SC_PLAYER_ALIVE_PACKET*>(p);
		int id = packet->id;
		auto& it = idANDtype.find(id);
		if (it == idANDtype.end()) break;
		else {

			Player_Character_Type type = it->second;
			reinterpret_cast<CyborgPlayer*>(m_ppPlayer[type])->SetCrawl(false);
			cout << "<Alive>" << type << endl;
		}
	}break;

	case SC_GO_STAGE2:
	{
		m_bChangeScene = true;
	}break;


	default: {
		printf("Scene[Stage1] - Unknown PACKET type [%d]\n", p[1]);
		break;
	}
	}
}

void CFirstRoundScene::Send_Go_Stage2()
{
	CS_GO_STAGE2_PACKET p;
	p.size = sizeof(p);
	p.type = CS_GO_STAGE2;
	send_packet(&p);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

void CSecondRoundScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int myPlayernum)
{

	CScene::BuildObjects(pd3dDevice, pd3dCommandList, myPlayernum);

	m_pUI = new CSecondRoundSceneUI();

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
	// Map (3)
	// [Present Setting]
	// 0 -	내부 사물			|| OBJ
	// 1 - 외벽 바운딩 박스		|| OBJ
	// 2 - 외벽&깃발			|| OBJ

	m_nHierarchicalGameObjects = 3;
	m_ppHierarchicalGameObjects = new CGameObject * [m_nHierarchicalGameObjects];

	// 1 - obj1
	CLoadedModelInfo* pInsideModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/map/Stage2InsideObj.bin", NULL);
	m_ppHierarchicalGameObjects[0] = new CStandardOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pInsideModel);
	if (pInsideModel) delete pInsideModel;

	CLoadedModelInfo* pOutlineBBModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/map/Stage2OutBB.bin", NULL);
	m_ppHierarchicalGameObjects[1] = new CStandardOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pOutlineBBModel);
	if (pOutlineBBModel) delete pOutlineBBModel;

	CLoadedModelInfo* pMainModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/map/Stage2.bin", NULL);
	m_ppHierarchicalGameObjects[2] = new CStandardOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pMainModel);
	if (pMainModel) delete pMainModel;


	// 2 - floor
	m_nFloorObj = 2;
	m_ppFloorObj = new CFloorObj * [m_nFloorObj];

	CLoadedModelInfo* pFloormodel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/map/Stage2Ground.bin", NULL);
	m_ppFloorObj[0] = new CFloorObj(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pFloormodel);
	m_ppFloorObj[0]->SetScale(3, 0, 3);
	m_ppFloorObj[0]->SetPosition(-100, 0.5, -500);
	if (pFloormodel) delete pFloormodel;

	CLoadedModelInfo* pOutmodel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/map/Stage2OutLine.bin", NULL);
	m_ppFloorObj[1] = new CFloorObj(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pOutmodel);
	if (pOutmodel) delete pOutmodel;



	//===============================//
	// Mission Obj
	m_nMissionObj = 5;
	m_ppMissionObj = new CMissonOBJ * [m_nMissionObj];

	// 미션 인식 범위
	XMFLOAT3 MissionRange = XMFLOAT3(10.f, 20.f, 10.f);

	// 협동 미션
	CLoadedModelInfo* pMssionMachine1 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/ObjModel/Mission/MissionMachine.bin", NULL);
	m_ppMissionObj[0] = new CMissonOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pMssionMachine1, MissionRange, 1);
	m_ppMissionObj[0]->Rotate(0.f, 180.f, 0.f);
	m_ppMissionObj[0]->SetPosition(MissionPos_Stage2[0]);
	if (pMssionMachine1) delete pMssionMachine1;

	CLoadedModelInfo* pMssionMachine2 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/ObjModel/Mission/MissionMachine.bin", NULL);
	m_ppMissionObj[1] = new CMissonOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pMssionMachine2, MissionRange, 1);
	m_ppMissionObj[1]->Rotate(0.f, 0.f, 0.f);
	m_ppMissionObj[1]->SetPosition(MissionPos_Stage2[1]);
	if (pMssionMachine2) delete pMssionMachine2;

	CLoadedModelInfo* pMssionMachine3 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/ObjModel/Mission/MissionMachine.bin", NULL);
	m_ppMissionObj[2] = new CMissonOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pMssionMachine3, MissionRange, 1);
	m_ppMissionObj[2]->Rotate(0.f, -90.f, 0.f);
	m_ppMissionObj[2]->SetPosition(MissionPos_Stage2[2]);
	if (pMssionMachine3) delete pMssionMachine3;

	CLoadedModelInfo* pMssionMachine4 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/ObjModel/Mission/MissionMachine.bin", NULL);
	m_ppMissionObj[3] = new CMissonOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pMssionMachine4, MissionRange, 1);
	m_ppMissionObj[3]->Rotate(0.f, -90.f, 0.f);
	m_ppMissionObj[3]->SetPosition(MissionPos_Stage2[3]);
	if (pMssionMachine4) delete pMssionMachine4;

	CLoadedModelInfo* pMssionMachine5 = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/ObjModel/Mission/MissionMachine.bin", NULL);
	m_ppMissionObj[4] = new CMissonOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pMssionMachine5, MissionRange, 1);
	m_ppMissionObj[4]->Rotate(0.f, -90.f, 0.f);
	m_ppMissionObj[4]->SetPosition(MissionPos_Stage2[4]);
	if (pMssionMachine5) delete pMssionMachine5;


	//===============================//

	CLoadedModelInfo* pRobotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Robot2.bin", NULL);
	m_pBoss = new CBossRobotObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pRobotModel, 7);
	m_pBoss->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	m_pBoss->SetPosition(110.f, 0, 205.f);
	m_pBoss->SetScale(8.0f, 8.0f, 8.0f);

	if (pRobotModel) delete pRobotModel;


	//===============================//
	m_nEnemy = 0;
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
	//m_nParticleObj = 1;
	//m_ppParticleObj = new CParticle * [m_nParticleObj];

	//for (int i = 0; i < m_nParticleObj; ++i) {
	//	m_ppParticleObj[i] = new CParticle(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, PlayerInitPos_Stage2[0]);
	//}

	//===============================//
	// SHADER OBJ (NULL)

	m_nShaders = 0;

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
		CyborgPlayer* pPlayer = new CyborgPlayer(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), m_pTerrain, m_ppModelInfoPlayer[i], THIRD_PERSON_CAMERA);
		m_ppPlayer[i] = pPlayer;
		m_ppPlayer[i]->SetPlayerData(i);
	}

	int playernum = 0;
	if (myPlayernum != 4) playernum = myPlayernum;

	m_pMyPlayer = m_ppPlayer[playernum];
	m_pMyPlayer->SetPosition(PlayerInitPos_Stage2[playernum]);
	m_pMyPlayer->ChangeCamera(SHOULDER_VIEW_CAMERA, 0.0f);
	m_pMyPlayer->m_bUnable = true;

	reinterpret_cast<CSecondRoundSceneUI*>(m_pUI)->m_fMaxStamina = reinterpret_cast<CyborgPlayer*>(m_pMyPlayer)->m_fMaxStaminer;

	CreateShaderVariables(pd3dDevice, pd3dCommandList);


#ifdef USE_NETWORK
	CS_GAMESTART_PACKET p;
	p.size = sizeof(p);
	p.type = CS_GAME_START;
	send_packet(&p);
#endif // USE_NETWORK
}

void CSecondRoundScene::ReleaseObjects()
{

	if (m_ppFloorObj)
	{
		for (int i = 0; i < m_nFloorObj; i++) if (m_ppFloorObj[i])
		{
			m_ppFloorObj[i]->ReleaseUploadBuffers();
			m_ppFloorObj[i]->Release();
		}
		delete[] m_ppFloorObj;
	}

	CScene::ReleaseObjects();
}

void CSecondRoundScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	CScene::Render(pd3dCommandList, pCamera);
	for (int i = 0; i < m_nFloorObj; i++)
	{
		if (m_ppFloorObj[i])
		{
			if (!m_ppFloorObj[i]->m_pSkinnedAnimationController) m_ppFloorObj[i]->UpdateTransform(NULL);
			m_ppFloorObj[i]->Animate(m_fElapsedTime);
			m_ppFloorObj[i]->Render(pd3dCommandList, pCamera);
		}
	}
	//for (int i = 0; i < m_nParticleObj; ++i) {
	//	if (m_ppParticleObj[i])
	//		m_ppParticleObj[i]->Render(pd3dCommandList, pCamera);
	//}
}

void CSecondRoundScene::ReleaseUploadBuffers()
{

	CScene::ReleaseUploadBuffers();

}

bool CSecondRoundScene::ProcessInput(HWND m_hWnd, POINT m_ptOldCursorPos, UCHAR* pKeysBuffer)
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

	if (pKeysBuffer['7'] & 0xF0) m_pMyPlayer->m_bUnable = true;


	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f) || (dwDirection1 != 0))
	{
		if ((cxDelta || cyDelta) && m_pMyPlayer->m_bUnable) {
			m_pMyPlayer->CameraRotate(0.0f, cxDelta, 0.0f);
			m_pMyPlayer->m_pCamera->RotatePitch(-cyDelta / 2.4);
		}


		if (dwDirection1 && m_pMyPlayer->m_bUnable) {


			if (reinterpret_cast<CSecondRoundSceneUI*>(m_pUI)->m_bMyOn == true) {
				CS_CHANGE_COMST_PACKET p;
				p.size = sizeof(p);
				p.type = CS_CHANGE_COMST;
				p.comNum = m_nDoingMachine;
				p.state = S2_COM_STATE::TURNOFF;
				send_packet(&p);
				reinterpret_cast<CSecondRoundSceneUI*>(m_pUI)->m_bMyOn = false;
			}

			if (!reinterpret_cast<CyborgPlayer*>(m_pMyPlayer)->m_bIsCrawl)
				m_pMyPlayer->Move(dwDirection1, m_pMyPlayer->GetVelocitySpeed(), true);
		}
	}

	m_dwLastDirection = dwDirection1;

	return false;
}

bool CSecondRoundScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{

	CScene::OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);

	switch (nMessageID)
	{

		//============ KeyDown ============//
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
		case 'K': {
			reinterpret_cast<CSecondRoundSceneUI*>(m_pUI)->m_bMyOn = false;
			m_pMyPlayer->m_bClear = true;
			m_pMyPlayer->m_xmf3BossPos = XMFLOAT3(m_pBoss->GetPosition().x * 2 - 10, 10, m_pBoss->GetPosition().z * 2 + 10);
			reinterpret_cast<CBossRobotObject*>(m_pBoss)->SetDiying(true);
			break;
		}
		case 'L': {
			CS_ALIVE_PLAYER_PACKET p;
			p.size = sizeof(p);
			p.type = CS_ALIVE_PLAYER;
			p.id = my_id;
			send_packet(&p);
			break;
		}
		case '1': {
			reinterpret_cast<CBossRobotObject*>(m_pBoss)->SetAttackStatus(true, 0);
			break;
		}
		case '2': {
			reinterpret_cast<CBossRobotObject*>(m_pBoss)->SetAttackStatus(true, 1);
			break;
		}
		case '3': {
			reinterpret_cast<CBossRobotObject*>(m_pBoss)->SetAttackStatus(true, 2);
			break;
		}
		case 'F':
		{
			if (!reinterpret_cast<CyborgPlayer*>(m_pMyPlayer)->m_bIsCrawl) {
				Player_Interaction_Type InteractionType = CheckInteraction();
				Interaction(InteractionType);
			}
			break;
		}
		break;
		}
		break;
	}

				   //============ KeyUP ============//
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

bool CSecondRoundScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{

	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		if (reinterpret_cast<CSecondRoundSceneUI*>(m_pUI)->m_bMyOn) //
		{
			::SetCapture(hWnd);
			::GetCursorPos(&m_ptOldCursorPos);

			S2_COM_STATE cur_state = reinterpret_cast<CSecondRoundSceneUI*>(m_pUI)->m_ppMachine[m_nDoingMachine]->GetState();

			S2_COM_STATE sstate = cur_state;
			if (reinterpret_cast<CSecondRoundSceneUI*>(m_pUI)->m_ppTagButton[0]->CheckMouseOn(hWnd, m_ptOldCursorPos))
			{
				sstate = TURNON;
			}
			else
			{
				sstate = TURNOFF;
			}


			if (cur_state != sstate) {
				CS_CHANGE_COMST_PACKET p;
				p.size = sizeof(p);
				p.type = CS_CHANGE_COMST;
				p.comNum = m_nDoingMachine;
				p.state = static_cast<S2_COM_STATE>(sstate);
				send_packet(&p);
			}


			::ReleaseCapture();
		}
		break;
	}
	return false;
}

bool CSecondRoundScene::CheckHeal()
{
	for (int i = 0; i < m_nPlayer; ++i) {
		if (m_pMyPlayer != m_ppPlayer[i] && m_pMyPlayer->m_xmBoundingBox.Intersects(m_ppPlayer[i]->m_xmBoundingBox)) {
			if (reinterpret_cast<CyborgPlayer*>(m_ppPlayer[i])->GetCurrentAni() == CRAWL) {
				reinterpret_cast<CyborgPlayer*>(m_pMyPlayer)->SetHealTarget(m_ppPlayer[i]->p_id);
				return true;
			}
		}
	}

	return false;
}

bool CSecondRoundScene::CheckFinalMission()
{
	for (int i = 0; i < m_nMissionObj; i++)
	{
		if (m_pMyPlayer->m_xmBoundingBox.Intersects(m_ppMissionObj[i]->m_xmMissionRange))
		{
			reinterpret_cast<CSecondRoundSceneUI*>(m_pUI)->m_bMyOn = true;
			m_nDoingMachine = i;
			return true;
		}
	}
	return false;
}

Player_Interaction_Type CSecondRoundScene::CheckInteraction()
{
	//if (CheckMissionObj())return CardMission;
	if (CheckHeal())return Heal;
	if (CheckFinalMission())return FinalMission;
	return NON;
}

void CSecondRoundScene::Interaction(Player_Interaction_Type type)
{
	switch (type) {
	case CardMission: {
		//reinterpret_cast<CyborgPlayer*>(m_pMyPlayer)->StartKeyMission(0);
		break;
	}
	case Heal: {
		int id = reinterpret_cast<CyborgPlayer*>(m_pMyPlayer)->GetHealTarget();
		CS_ALIVE_PLAYER_PACKET p;
		p.size = sizeof(p);
		p.type = CS_ALIVE_PLAYER;
		p.id = id;
		send_packet(&p);
		break;
	}
	case FinalMission: {
		break;
	}
	}
}

void CSecondRoundScene::AnimateObjects(float fTimeElapsed)
{
	CScene::AnimateObjects(fTimeElapsed);

	if (reinterpret_cast<CyborgPlayer*>(m_pMyPlayer)->m_fStaminer < reinterpret_cast<CyborgPlayer*>(m_pMyPlayer)->m_fMaxStaminer)
	{
		reinterpret_cast<CSecondRoundSceneUI*>(m_pUI)->m_bStaminaBarOn = true;
		reinterpret_cast<CSecondRoundSceneUI*>(m_pUI)->m_fStaminaRange = reinterpret_cast<CyborgPlayer*>(m_pMyPlayer)->m_fStaminer;
	}
	else reinterpret_cast<CSecondRoundSceneUI*>(m_pUI)->m_bStaminaBarOn = false;


	//for (int i = 0; i < m_nMissionObj; i++)
	//{
	//	Missionflag = false;
	//	
	//	if (CheckMissionBound(m_pMyPlayer, m_ppMissionObj[i]))
	//	{
	//		Missionflag = true;
	//		
	//	}
	//	m_ppMissionObj[i]->m_bMissionflag = Missionflag;
	//}

	if (reinterpret_cast<CBossRobotObject*> (m_pBoss)->GetDiy() == 2) {
		reinterpret_cast<CSecondRoundSceneUI*>(m_pUI)->m_bEnd = true;
	}

	int check = 0;
	for (int i = 0; i < m_nMissionObj; i++)
	{
		if (reinterpret_cast<CSecondRoundSceneUI*>(m_pUI)->m_ppMachine[i]->GetState() == TURNON) check += 1;
	}

	if (m_nMissionLevel == 3) {
		reinterpret_cast<CSecondRoundSceneUI*>(m_pUI)->m_bMyOn = false;
		m_pMyPlayer->m_bClear = true;
		m_pMyPlayer->m_xmf3BossPos = XMFLOAT3(m_pBoss->GetPosition().x * 2 - 10, 10, m_pBoss->GetPosition().z * 2 + 10);
		reinterpret_cast<CBossRobotObject*>(m_pBoss)->SetDiying(true);
	}

	if (check > 1 && m_nMissionLevel < 3) {
		if (reinterpret_cast<CSecondRoundSceneUI*>(m_pUI)->m_fMissionRange[m_nMissionLevel] < 650)
		{
			reinterpret_cast<CSecondRoundSceneUI*>(m_pUI)->SetProgress(m_nMissionLevel, 1.5f);
		}
		if (reinterpret_cast<CSecondRoundSceneUI*>(m_pUI)->m_ppProgressBar[m_nMissionLevel]->GetDone() && m_nMissionLevel < 3)
		{
			m_nMissionLevel += 1;
		}
	}
	else {
		if (reinterpret_cast<CSecondRoundSceneUI*>(m_pUI)->m_fMissionRange[m_nMissionLevel] > 0 && reinterpret_cast<CSecondRoundSceneUI*>(m_pUI)->m_fMissionRange[m_nMissionLevel] < 650)
		{
			reinterpret_cast<CSecondRoundSceneUI*>(m_pUI)->SetProgress(m_nMissionLevel, -0.7f);
		}
	}

}

void CSecondRoundScene::ProcessPacket(char* p)
{

	switch (p[1])
	{
	case SC_ADD_PLAYER:
	{
		SC_ADD_PLAYER_PACKET* packet = reinterpret_cast<SC_ADD_PLAYER_PACKET*>(p);
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
		int n_id = packet->id - 200;
		XMFLOAT3 half_pos{ packet->position.x / 2, 0.f, packet->position.z / 2 };
		reinterpret_cast<CBossRobotObject*>(m_pBoss)->SetPosition(half_pos);
		// reinterpret_cast<CRobotObject*>(m_ppEnemy[n_id])->SetTarget(m_ppEnemy[n_id]->GetPosition());

	}
				   break;

	case SC_MOVE_NPC: {
		SC_MOVE_NPC_PACKET* packet = reinterpret_cast<SC_MOVE_NPC_PACKET*>(p);

		int n_id = packet->id - 200;
		XMFLOAT3 half_pos{ packet->next_pos.x / 2, 0.f, packet->next_pos.z / 2 };
		reinterpret_cast<CBossRobotObject*>(m_pBoss)->SetTarget(half_pos);
	}
					break;

	case SC_ATTACK_NPC: {
		SC_ATTACK_NPC_PACKET* packet = reinterpret_cast<SC_ATTACK_NPC_PACKET*>(p);


		int id = packet->p_id;
		auto& it = idANDtype.find(id);
		XMFLOAT3 xmf3{};
		if (it == idANDtype.end()) break;
		else {
			Player_Character_Type type = it->second;
			xmf3 = reinterpret_cast<CyborgPlayer*>(m_ppPlayer[type])->GetPosition();
		}

		reinterpret_cast<CBossRobotObject*>(m_pBoss)->SetAttackStatus(true, 2);
		XMFLOAT3 double_pos{ m_pBoss->GetPosition().x * 2.f, 0.f, m_pBoss->GetPosition().z * 2.f };

		CS_NPC_UPDATE_PACKET nup;
		nup.size = sizeof(nup);
		nup.type = CS_NPC_UPDATE;
		nup.n_id = packet->n_id;
		nup.position = double_pos;
		send_packet(&nup);

	}break;

	case SC_NPC_UPDATE: {
		SC_NPC_UPDATE_PACKET* packet = reinterpret_cast<SC_NPC_UPDATE_PACKET*>(p);
		XMFLOAT3 half_pos{ packet->position.x / 2.f, 0.f, packet->position.z / 2.f };
		m_pBoss->SetPosition(half_pos);
	}break;

	case SC_PLAYER_DEATH: {
		SC_PLAYER_DEATH_PACKET* packet = reinterpret_cast<SC_PLAYER_DEATH_PACKET*>(p);

		int id = packet->id;
		auto& it = idANDtype.find(id);
		if (it == idANDtype.end()) break;
		else {

			Player_Character_Type type = it->second;
			reinterpret_cast<CyborgPlayer*>(m_ppPlayer[type])->SetCrawl(true);
		}

		if (packet->id == my_id)
			reinterpret_cast<CSecondRoundSceneUI*>(m_pUI)->m_bMyOn = false;


	}break;

	case SC_PLAYER_ALIVE: {
		SC_PLAYER_ALIVE_PACKET* packet = reinterpret_cast<SC_PLAYER_ALIVE_PACKET*>(p);
		int id = packet->id;
		auto& it = idANDtype.find(id);
		if (it == idANDtype.end()) break;
		else {

			Player_Character_Type type = it->second;
			reinterpret_cast<CyborgPlayer*>(m_ppPlayer[type])->SetCrawl(false);
		}
	}break;

	case SC_CHANGE_COMST: {
		SC_CHANGE_COMST_PACKET* packet = reinterpret_cast<SC_CHANGE_COMST_PACKET*>(p);
		S2_COM_STATE st = static_cast<S2_COM_STATE>(packet->state);
		reinterpret_cast<CSecondRoundSceneUI*>(m_pUI)->m_ppMachine[packet->comNum]->SetState(st);

	}break;

	default:
		printf("Scene[Stage2] - Unknown PACKET type [%d]\n", p[1]);
	}
}
