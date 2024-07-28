
#include "stdafx.h"
#include "PrepareScene.h"

void CPrepareRoomScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int myPlayernum)
{
	CScene::BuildObjects(pd3dDevice, pd3dCommandList, myPlayernum);




	m_pUI = new CPrepareRoomSceneUI();

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
	m_ppHierarchicalGameObjects[0]->SetPosition(-80.f, 0.f, 40.f);
	if (pContainerModel) delete pContainerModel;


	CLoadedModelInfo* pMiddleContainer = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/ObjModel/MiddleContainer.bin", NULL);
	m_ppHierarchicalGameObjects[1] = new CStandardOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pMiddleContainer);
	m_ppHierarchicalGameObjects[1]->SetPosition(-40.f, 0.f, 5.f);

	m_ppHierarchicalGameObjects[2] = new CStandardOBJ(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pMiddleContainer);
	m_ppHierarchicalGameObjects[2]->SetPosition(40.f, 0.f, 5.f);
	if (pMiddleContainer) delete pMiddleContainer;


	// Floor
	m_nFloorObj = 1;
	m_ppFloorObj = new CGameObject * [m_nFloorObj];

	CLoadedModelInfo* pFloormodel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/ObjModel/StandardFloor.bin", NULL);
	m_ppFloorObj[0] = new CFloorObj(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pFloormodel);
	m_ppFloorObj[0]->SetScale(3.f, 0.f, 3.f);
	m_ppFloorObj[0]->SetPosition(-30, 0.2, -10);
	if (pFloormodel) delete pFloormodel;

	//===============================//
	// Player
	m_nPlayer = 12;
	m_ppPlayer = new CPlayer * [m_nPlayer];
	m_ppModelInfoPlayer = new CLoadedModelInfo * [m_nPlayer]; // Play Character

	for (int j = 0; j < 3; j++)
	{
		m_ppModelInfoPlayer[j * 4] = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), "Model/Player_1.bin", NULL);
		m_ppModelInfoPlayer[1 + j * 4] = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), "Model/Player_2.bin", NULL);
		m_ppModelInfoPlayer[2 + j * 4] = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), "Model/Player_3.bin", NULL);
		m_ppModelInfoPlayer[3 + j * 4] = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), "Model/Robot.bin", NULL);
	}

	for (int j = 0; j < 3; j++)
	{
		for (int i = 0; i < 4; ++i) {
			CyborgPlayer* pPlayer = new CyborgPlayer(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), m_pTerrain, m_ppModelInfoPlayer[i + j * 4]);
			m_ppPlayer[i + j * 4] = pPlayer;
			m_ppPlayer[i + j * 4]->SetPlayerData(i);
			m_ppPlayer[i + j * 4]->Rotate(0.f, 180.f, 0.f);
			m_ppPlayer[i + j * 4]->m_bUnable = false;
		}
	}

	SetPlayer();


	m_pMyPlayer = m_ppPlayer[4];
	m_pMyPlayer->ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);

#ifdef USE_NETWORK
	CS_ENTER_ROOM_PACKET p;
	p.size = sizeof(p);
	p.type = CS_ENTER_ROOM;
	send_packet(&p);
#endif // USE_NETWORK

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

	DWORD dwDirection = 0;
	DWORD dwDirection1 = 0;

	if (pKeysBuffer['W'] & 0xF0) dwDirection1 |= DIR_FORWARD;
	if (pKeysBuffer['S'] & 0xF0) dwDirection1 |= DIR_BACKWARD;
	if (pKeysBuffer['A'] & 0xF0) dwDirection1 |= DIR_LEFT;
	if (pKeysBuffer['D'] & 0xF0) dwDirection1 |= DIR_RIGHT;

	if ((cxDelta != 0.0f) || (cyDelta != 0.0f) || (dwDirection1 != 0))
	{
		if ((cxDelta || cyDelta) && m_pMyPlayer->m_bUnable)
			m_pMyPlayer->CameraRotate(cyDelta, cxDelta, 0.0f);

		if (dwDirection1 && m_pMyPlayer->m_bUnable) {
			// reinterpret_cast<CyborgPlayer*>(m_pMyPlayer)->StartKeyMission(-1);
			m_pMyPlayer->Move(dwDirection1, 4.f, false);
		}
	}

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
			m_bChangeScene = true;
#endif // USE_NETWORK
			break;
		}
		case '1': {
			select = Corzim;
#ifdef USE_NETWORK
			CS_CHANGE_CHARACTER_PACKET p;
			p.size = sizeof(p);
			p.type = CS_CHANGE_CHARACTER;
			p.c_type = select;
			send_packet(&p);
#else
			ChangeModel(0, select);
#endif // USE_NETWORK
			break;
		}
		case '2': {
			select = Evan;
#ifdef USE_NETWORK
			CS_CHANGE_CHARACTER_PACKET p;
			p.size = sizeof(p);
			p.type = CS_CHANGE_CHARACTER;
			p.c_type = select;
			send_packet(&p);
#else
			ChangeModel(0, select);
#endif // USE_NETWORK
			break;
		}
		case '3': {
			select = Uranya;
#ifdef USE_NETWORK
			CS_CHANGE_CHARACTER_PACKET p;
			p.size = sizeof(p);
			p.type = CS_CHANGE_CHARACTER;
			p.c_type = select;
			send_packet(&p);
#else
			ChangeModel(0, select);
#endif // USE_NETWORK
			break;
		}

		default:
			break;
		}

	}
	}
	CScene::OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
	return false;
}

void CPrepareRoomScene::ReleaseObjects()
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

void CPrepareRoomScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
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

void CPrepareRoomScene::ReleaseUploadBuffers()
{
	CScene::ReleaseUploadBuffers();
	for (int i = 0; i < m_nFloorObj; i++) m_ppFloorObj[i]->ReleaseUploadBuffers();


}

bool CPrepareRoomScene::AllPlayerReady()
{
	if (m_pMyPlayer->m_bReady)
	{
		return true;
	}
	else return false;
}

void CPrepareRoomScene::SetPlayer()
{

	for (int i = 0; i < 3; i++)
	{
		m_ppPlayer[0 + i * 4]->SetPosition(XMFLOAT3(-15.f + i * 15, 0.f, 0.f));
		m_ppPlayer[1 + i * 4]->SetPosition(XMFLOAT3(-15.f + i * 15, 0.f, 0.f));
		m_ppPlayer[2 + i * 4]->SetPosition(XMFLOAT3(-15.f + i * 15, 0.f, 0.f));
		m_ppPlayer[3 + i * 4]->SetPosition(XMFLOAT3(-15.f + i * 15, 0.f, 0.f));
	}

	for (int i = 1; i < 4; i++)
	{
		m_ppPlayer[4 * i - 1]->m_bUnable = true;
	}


}


void CPrepareRoomScene::ChangeModel(int nPlayer, int nModel)
{

	switch (nPlayer)
	{
	case 0:
		m_nPlayerSet[0] = nModel;
		m_ppPlayer[nModel]->m_bUnable = true;
		for (int i = 0; i < 4; i++)	if (nModel != i) m_ppPlayer[i]->m_bUnable = false;
		break;

	case 1:
		m_nPlayerSet[1] = nModel;
		m_ppPlayer[nModel + 4]->m_bUnable = true;
		for (int i = 4; i < 8; i++)	if (nModel + 4 != i) m_ppPlayer[i]->m_bUnable = false;
		break;

	case 2:
		m_nPlayerSet[2] = nModel;
		m_ppPlayer[nModel + 8]->m_bUnable = true;
		for (int i = 8; i < 12; i++) if (nModel + 8 != i) m_ppPlayer[i]->m_bUnable = false;
		break;
	}

}


int CPrepareRoomScene::GetModelInfo()
{
	int playerId = 0;
	if (my_id != -1) playerId = my_id;
	return m_nPlayerSet[playerId];
}


void CPrepareRoomScene::ProcessPacket(char* p)
{
	switch (p[1])
	{
	//case SC_LOGIN_INFO:
	//{
	//	/*SC_LOGIN_INFO_PACKET* packet = reinterpret_cast<SC_LOGIN_INFO_PACKET*>(p);
	//	my_id = packet->id;
	//	cout << "My ID is " << my_id << " !" << endl;
	//	reinterpret_cast<CFirstSceneUI*>(m_pUI)->m_bPlayerOn[my_id] = true;*/

	//} break;

	case SC_CHANGE_CHARACTER: {
		SC_CHANGE_CHARACTER_PACKET* packet = reinterpret_cast<SC_CHANGE_CHARACTER_PACKET*>(p);
		// cout << packet->id << " -> " << packet->c_type << endl;
		ChangeModel(packet->id, packet->c_type);
		reinterpret_cast<CPrepareRoomSceneUI*>(m_pUI)->m_bPlayerOn[packet->id] = true;
		reinterpret_cast<CPrepareRoomSceneUI*>(m_pUI)->m_text[packet->id] = string_to_wstring(packet->name);
	}break;

	case SC_GAME_START: {
		m_pMyPlayer->m_bReady = !m_pMyPlayer->m_bReady;
		m_bChangeScene = true;
		// 플레이게임으로 씬전환
		break;
	}


	default:
		printf("Scene[LOBBY] - Unknown PACKET type [%d]\n", p[1]);
		break;
	}
}



