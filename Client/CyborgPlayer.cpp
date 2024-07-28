#include "stdafx.h"
#include "CyborgPlayer.h"

CyborgPlayer::CyborgPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext, CLoadedModelInfo* pModel, DWORD dCamera, int nMaxAni)
{
	m_pCamera = ChangeCamera(dCamera, 0.0f);

	CLoadedModelInfo* pPlayerModel = pModel;

	SetChild(pPlayerModel->m_pModelRootObject, true);

	m_pSkinnedAnimationController = new CAnimationController(pd3dDevice, pd3dCommandList, nMaxAni, pPlayerModel);

	// Animation Setting
	for (int i = 0; i < nMaxAni; ++i)
		m_pSkinnedAnimationController->SetTrackAnimationSet(i, i);

	// Default animation setting
	m_pSkinnedAnimationController->SetAllTrackDisable();
	m_pSkinnedAnimationController->SetTrackEnable(IDLE, true);
	m_pasCurrentAni = IDLE;

	// Sound setting
	m_pSkinnedAnimationController->SetCallbackKeys(1, 2);
	m_pSkinnedAnimationController->SetCallbackKeys(2, 2);
#ifdef _WITH_SOUND_RESOURCE
	m_pSkinnedAnimationController->SetCallbackKey(0, 0.1f, _T("Footstep01"));
	m_pSkinnedAnimationController->SetCallbackKey(1, 0.5f, _T("Footstep02"));
	m_pSkinnedAnimationController->SetCallbackKey(2, 0.9f, _T("Footstep03"));
#else
	//m_pSkinnedAnimationController->SetCallbackKey(1, 0, 0.3f, _T("Sound/Footstep01.wav"));
	//m_pSkinnedAnimationController->SetCallbackKey(1, 1, 0.8f, _T("Sound/Footstep02.wav"));	

	//m_pSkinnedAnimationController->SetCallbackKey(2, 0, 0.15f, _T("Sound/Footstep01.wav"));
	//m_pSkinnedAnimationController->SetCallbackKey(2, 1, 0.5f, _T("Sound/Footstep02.wav"));
		//m_pSkinnedAnimationController->SetCallbackKey(1, 2, 0.39f, _T("Sound/Footstep03.wav"));
#endif
	//CAnimationCallbackHandler* pAnimationCallbackHandler = new CSoundCallbackHandler();
	////CAnimationCallbackHandler* pAnimationCallbackHandler1 = new CSoundCallbackHandler();
	//m_pSkinnedAnimationController->SetAnimationCallbackHandler(1, pAnimationCallbackHandler);
	//m_pSkinnedAnimationController->SetAnimationCallbackHandler(2, pAnimationCallbackHandler);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	SetPlayerUpdatedContext(pContext);
	SetCameraUpdatedContext(pContext);

	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;
	if(pContext)SetPosition(XMFLOAT3(100.0f, pTerrain->GetHeight(310.0f, 590.0f), 300.0f));
	SetScale(XMFLOAT3(10.0f, 10.0f, 10.0f));

	CLoadedModelInfo* pMiddleContainer = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Mission_1/MissionBox_1.bin", NULL);
	m_pMissionBoxObject = new Mission(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pMiddleContainer);
	
	m_nMissionObject = 8;
	
	char** FileName = new char* [m_nMissionObject];
	FileName[0] = "Model/Mission_1/Mission1_1.bin";
	FileName[1] = "Model/Mission_1/Mission1_2.bin";
	FileName[2] = "Model/Mission_1/Mission1_3.bin";
	FileName[3] = "Model/Mission_1/Mission1_4.bin";
	FileName[4] = "Model/Mission_1/Mission1_5.bin";
	FileName[5] = "Model/Mission_1/Mission1_6.bin";
	FileName[6] = "Model/Mission_1/Mission1_7.bin";
	FileName[7] = "Model/Mission_1/Mission1_8.bin";

	m_ppMissionObjects = new CGameObject * [m_nMissionObject+1];
	m_ppMissionAnswer = new CGameObject * [m_nMissionObject];

	for (int i = 0; i < m_nMissionObject; ++i) {
		CLoadedModelInfo* pMission = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, FileName[i], NULL);
		m_ppMissionObjects[i] = new Mission(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pMission);
		
		CLoadedModelInfo* pAnswer = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, FileName[i], NULL);
		m_ppMissionAnswer[i] = new Mission(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pAnswer);
	}

	CLoadedModelInfo* pMission = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Mission_1/CardKey.bin", NULL);
	m_ppMissionObjects[8] = new Mission(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pMission);
	m_ppMissionObjects[8]->Rotate(0, 0, 70);
}

CyborgPlayer::~CyborgPlayer()
{
}

CCamera* CyborgPlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return(m_pCamera);
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		SetFriction(250.0f);
		SetGravity(XMFLOAT3(0.0f, -400.0f, 0.0f));
		SetMaxVelocityXZ(300.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->ChangeView(true);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.5f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case SPACESHIP_CAMERA:
		SetFriction(125.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(300.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(SPACESHIP_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->ChangeView(false);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 200.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case THIRD_PERSON_CAMERA:
		SetFriction(130.0f);
		SetGravity(XMFLOAT3(0.0f, -250.0f, 0.0f));
		SetMaxVelocityXZ(20.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.05f);
		m_pCamera->ChangeView(false);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, -40.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case PREPARE_ROOM_CAMERA:
		SetFriction(130.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(0.0f);
		SetMaxVelocityY(0.0f);
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.05f);
		m_pCamera->ChangeView(false);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 15.0f, -40.0f));
		m_pCamera->SetLookAt(XMFLOAT3(0.f, 15.f, 0.f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case SHOULDER_VIEW_CAMERA:
		SetFriction(130.0f);
		SetGravity(XMFLOAT3(0.0f, -250.0f, 0.0f));
		SetMaxVelocityXZ(20.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.05f);
		m_pCamera->ChangeView(true);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 22.5f, -15.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	default:
		break;
	}
	m_pCamera->SetPosition(Vector3::Add(m_xmf3Position, m_pCamera->GetOffset()));
	Update(fTimeElapsed);

	return(m_pCamera);
}

void CyborgPlayer::OnPlayerUpdateCallback(float fTimeElapsed)
{
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)m_pPlayerUpdatedContext;
	if (m_pPlayerUpdatedContext) {
		XMFLOAT3 xmf3Scale = pTerrain->GetScale();
		XMFLOAT3 xmf3PlayerPosition = GetPosition();
		int z = (int)(xmf3PlayerPosition.z / xmf3Scale.z);
		bool bReverseQuad = ((z % 2) != 0);
		float fHeight = pTerrain->GetHeight(xmf3PlayerPosition.x, xmf3PlayerPosition.z, bReverseQuad) + 0.0f;
		if (xmf3PlayerPosition.y < fHeight)
		{
			XMFLOAT3 xmf3PlayerVelocity = GetVelocity();
			xmf3PlayerVelocity.y = 0.0f;
			SetVelocity(xmf3PlayerVelocity);
			xmf3PlayerPosition.y = fHeight;
			SetPosition(xmf3PlayerPosition);
		}
	}
}

void CyborgPlayer::OnCameraUpdateCallback(float fTimeElapsed)
{
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)m_pCameraUpdatedContext;
	XMFLOAT3 xmf3Scale = pTerrain->GetScale();
	XMFLOAT3 xmf3CameraPosition = m_pCamera->GetPosition();
	int z = (int)(xmf3CameraPosition.z / xmf3Scale.z);
	bool bReverseQuad = ((z % 2) != 0);
	float fHeight = pTerrain->GetHeight(xmf3CameraPosition.x, xmf3CameraPosition.z, bReverseQuad) + 5.0f;
	if (xmf3CameraPosition.y <= fHeight)
	{
		xmf3CameraPosition.y = fHeight;
		m_pCamera->SetPosition(xmf3CameraPosition);
		if (m_pCamera->GetMode() == THIRD_PERSON_CAMERA)
		{
			CThirdPersonCamera* p3rdPersonCamera = (CThirdPersonCamera*)m_pCamera;
			p3rdPersonCamera->SetLookAt(GetPosition());
		}
	}
}


// 1
void CyborgPlayer::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity)
{

	if (dwDirection)
	{
		IsWalk();
		IsRun();
		IsCreep();
	}

	CPlayer::Move(dwDirection, fDistance, bUpdateVelocity);
}

void CyborgPlayer::Update(float fTimeElapsed)
{
	CPlayer::Update(fTimeElapsed);
	if (m_pMissionBoxObject)
	{
		if (!m_pMissionBoxObject->m_pSkinnedAnimationController) {

			if (reinterpret_cast<Mission*>(m_pMissionBoxObject)->GetUnable()) {
				XMFLOAT3 Height = { 0.f,16.f, 0.f };
				XMFLOAT3 Look = Vector3::ScalarProduct(GetLook(), 7.0);
				m_bSecurityKey ? Height.y = 21.5, Look.x=0,Look.y=0,Look.z=0 : Height.y = 16;
				m_pMissionBoxObject->SetPosition(
					Vector3::Add(Vector3::Add(GetPosition(), Look), Height));
				float angle = Vector3::Angle(
					XMFLOAT3(-m_pMissionBoxObject->GetLook().x, m_pMissionBoxObject->GetLook().y, -m_pMissionBoxObject->GetLook().z),
					Vector3::Minus(XMFLOAT3(GetPosition().x, m_pMissionBoxObject->GetPosition().y, GetPosition().z),
						m_pMissionBoxObject->GetPosition()));

				if (!m_bSecurityKey) {
					if (angle >= 0.1)
						m_pMissionBoxObject->Rotate(0, angle / 2, 0);
					m_ppSelectMission[0]->SetPosition(Vector3::Add(Vector3::Add(m_pMissionBoxObject->GetPosition(), XMFLOAT3(0, 1.7f, 0)), m_pMissionBoxObject->GetRight(), -2.0f));
					m_ppSelectMission[1]->SetPosition(Vector3::Add(m_pMissionBoxObject->GetPosition(), XMFLOAT3(0, 1.7f, 0)));
					m_ppSelectMission[2]->SetPosition(Vector3::Add(Vector3::Add(m_pMissionBoxObject->GetPosition(), XMFLOAT3(0, 1.7f, 0)), m_pMissionBoxObject->GetRight(), 2.0f));
					m_ppSelectMission[3]->SetPosition(Vector3::Add(m_pMissionBoxObject->GetPosition(), XMFLOAT3(0, 4.2f, 0)));
					for (int i = 0; i < 4; ++i) {
						i == 3 ? m_ppSelectMission[i]->Rotate(0.f, 0.3f, 0.f) : m_ppSelectMission[i]->Rotate(0.f, -0.3f, 0.f);
						m_ppSelectMission[i]->Animate(fTimeElapsed);
					}
				}
				else {
					XMFLOAT3 Up = XMFLOAT3{ 0.7,0.3,0 };
					m_pMissionBoxObject->Rotate(&Up,0.5f);
					m_fKeyRotate >= 270 ? 
						m_fKeyRotate = 0, reinterpret_cast<Mission*>(m_pMissionBoxObject)->SetUnable(false) : m_fKeyRotate += 0.5f;
				}
				m_pMissionBoxObject->Animate(fTimeElapsed);
			}
		}
	}
	UpdateBB();
	if (m_pSkinnedAnimationController)
	{
		AnimationBlending(m_pasCurrentAni, m_pasNextAni);
		if (m_pasNextAni == RUN) SetMaxVelocityXZ(30.f);
		else if (m_pasNextAni == WALK) SetMaxVelocityXZ(15.f);
		else if (m_pasNextAni == CREEP) SetMaxVelocityXZ(10.f);
		ExhaustionStaminer();
		RestorationStaminer();

		float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
		if (::IsZero(fLength))
		{
			if(!m_bIsCrawl)IsIdle();
			else { IsCrawl(); }
		}
	}

	CS_UPDATE_PLAYER_PACKET Upacket;
	Upacket.size = sizeof(Upacket);
	Upacket.type = CS_UPDATE_PLAYER;
	Upacket.position = GetPosition();
	Upacket.rotate = DirectX::XMFLOAT3(m_fPitch, m_fYaw, m_fRoll);
	Upacket.ani_st = m_pasCurrentAni;
	SetBuffer(&Upacket, Upacket.size);
}

void CyborgPlayer::Release()
{
	CPlayer::Release();
	if (m_ppMissionObjects) {
		for (int i = 0; i < m_nMissionObject; ++i)
			m_ppMissionObjects[i]->Release();
		//delete[] m_ppMissionObjects;
	}
}

void CyborgPlayer::UpdateBB()
{
	if (m_pSibling)m_pSibling->UpdateBoundingBox(m_xmf3Velocity);
	if (m_pChild)m_pChild->UpdateBoundingBox(m_xmf3Velocity);
}

void CyborgPlayer::AnimationBlending(Player_Animation_ST type1, Player_Animation_ST type2)
{
	// 섞인 애니메이션
	if (m_pSkinnedAnimationController->m_fBlendingTime <= 1.0f && type2 != NONE) {
		// If need to blending
		// 체인지 애니메이션(섞인거) 
		m_pSkinnedAnimationController->SetAnimationBlending(true); // 블렌딩을 할지 말지
		m_pSkinnedAnimationController->SetTrackBlending(m_pasCurrentAni, type2); // 몇번째랑 몇번째
	}
	else { // 무조건 WALK
		if (type2 != NONE)m_pasCurrentAni = type2;
		m_pSkinnedAnimationController->SetTrackEnable(m_pasCurrentAni, true);
		m_pSkinnedAnimationController->SetAnimationBlending(false);
		m_pasNextAni = NONE;
	}

}

int CyborgPlayer::Random(int min, int max)
{
	uniform_int_distribution uid{ min, max };
	return uid(m_m64RandomEngine);
}

int CyborgPlayer::DuplicationCheck(int num, int* target, int end)
{
	for (int i = 0; i < end; ++i)
		if (num == target[i])return 0;
	return 1;
}

void CyborgPlayer::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	CPlayer::Render(pd3dCommandList, pCamera);
	if (reinterpret_cast<Mission*>(m_pMissionBoxObject)->GetUnable()) {
		m_pMissionBoxObject->Render(pd3dCommandList, pCamera);
		if (!m_bSecurityKey) {
			for (int i = 0; i < 4; ++i) {
				m_ppSelectMission[i]->Render(pd3dCommandList, pCamera);
			}
		}
	}
}

bool CyborgPlayer::StartKeyMission(int type)
{
	switch (type) {
	case 0: {
		if (!reinterpret_cast<Mission*>(m_pMissionBoxObject)->GetUnable()) {
			m_nAnswerCount = 0;
			if (m_ppSelectMission)delete[] m_ppSelectMission;
			ChangeCamera(FIRST_PERSON_CAMERA, 0.0f);
			reinterpret_cast<Mission*>(m_pMissionBoxObject)->SetUnable(true);
			int* k = new int[3];
			for (int i = 0; i < 3; ++i) {
				k[i] = Random(0, 7);
				if (DuplicationCheck(k[i], k, i)==0)--i;
			}
			m_ppSelectMission = new CGameObject * [4];
			for (int i = 0; i < 3; ++i) {
				m_ppSelectMission[i] = m_ppMissionObjects[k[i]];
			}
			m_nAnswer = Random(0, 2);
			m_ppSelectMission[3] = m_ppMissionAnswer[k[m_nAnswer]];
			delete[] k;
		}
		break;
	}
	case 1: {
			if (m_ppSelectMission)delete[] m_ppSelectMission;
			ChangeCamera(FIRST_PERSON_CAMERA, 0.0f);
			reinterpret_cast<Mission*>(m_pMissionBoxObject)->SetUnable(true);
			int* k = new int[3];
			for (int i = 0; i < 3; ++i) {
				k[i] = Random(0, 7);
				if (DuplicationCheck(k[i], k, i) == 0)--i;
			}
			m_ppSelectMission = new CGameObject * [4];
			for (int i = 0; i < 3; ++i) {
				m_ppSelectMission[i] = m_ppMissionObjects[k[i]];
			}
			m_nAnswer = Random(0, 2);
			m_ppSelectMission[3] = m_ppMissionAnswer[k[m_nAnswer]];
			delete[] k;
		break;
	}
	case 2: {
		//if (reinterpret_cast<Mission*>(m_pMissionBoxObject)->GetUnable()) {
			ChangeCamera(SHOULDER_VIEW_CAMERA, 0.0f);
			if (m_pMissionBoxObject)delete m_pMissionBoxObject;
			m_pMissionBoxObject = m_ppMissionObjects[8];
			reinterpret_cast<Mission*>(m_pMissionBoxObject)->SetUnable(true);
			//reinterpret_cast<Mission*>(m_pMissionBoxObject)->SetUnable(false);
		//}
		break;
	}
	default: {
		if (reinterpret_cast<Mission*>(m_pMissionBoxObject)->GetUnable()&&!m_bSecurityKey) {
			ChangeCamera(SHOULDER_VIEW_CAMERA, 0.0f);
			reinterpret_cast<Mission*>(m_pMissionBoxObject)->SetUnable(false);
		}
		break;
	}
	}

	return false;
}

void CyborgPlayer::MissionCheck(int num)
{
	if (reinterpret_cast<Mission*>(m_pMissionBoxObject)->GetUnable()) {
		cout << m_nAnswer << ", " << num << endl;
		if (m_nAnswer == num) {
			++m_nAnswerCount;
			//m_nAnswerCount == 3 ? m_bSecurityKey = true, StartKeyMission(2) : StartKeyMission(1);
		
			if (m_nAnswerCount == 3) {
				m_bSecurityKey = true;
				StartKeyMission(2);
				CS_GETKEY_PACKET packet;
				packet.size = sizeof(packet);
				packet.type = CS_GETKEY;
				SetBuffer(&packet, packet.size);
			}
			else {
				StartKeyMission(1);
			}

		}
		else
			StartKeyMission(-1);
	}
}

void CyborgPlayer::ExhaustionStaminer()
{
	if (m_bIsRun)
		if(m_fStaminer>0)m_fStaminer-=0.1f;
	if (m_bIsCreep)
		if (m_fStaminer > 0)m_fStaminer -= 0.1f;
	if (m_fStaminer <= 0.0f) {
		m_bHasStaminer = false;
		m_bIsRun = false;
		m_bIsCreep = false;
		m_bIsCreep_flag = false;
	}
}

void CyborgPlayer::RestorationStaminer()
{
	if (!m_bIsRun && !m_bIsCreep)
		if (m_fStaminer < m_fMaxStaminer)m_fStaminer+=0.1f;
	if (m_fStaminer >= 20.0f) {
		m_bHasStaminer = true;
	}
}

void CyborgPlayer::SetRun(bool value)
{
	m_bIsRun = value;
	m_bIsCreep = false;
	m_bIsCreep_flag = false;
}

void CyborgPlayer::SetCreep()
{
	if (m_bIsCreep == m_bIsCreep_flag)
		m_bIsCreep = !m_bIsCreep;
}

void CyborgPlayer::SetCreepFlag()
{
	m_bIsCreep_flag = m_bIsCreep;
}

void CyborgPlayer::SetJump()
{
}

void CyborgPlayer::SetCrawl(bool IsCrawl)
{
	m_bIsCrawl = IsCrawl;
	m_xmBoundingBox = BoundingOrientedBox(GetPosition(), XMFLOAT3(0.3f, 0.3f, 0.3f), XMFLOAT4(0.f, 0.f, 0.f, 1.0f));;
}

void CyborgPlayer::IsIdle()
{
	if (m_pasCurrentAni != IDLE && m_pSkinnedAnimationController->m_fBlendingTime >= 1.0f) {
		m_bIsRun = false;
		m_bIsCreep = false;
		m_pasNextAni = IDLE;
		m_pSkinnedAnimationController->m_fBlendingTime = 0.0f;

		AnimationPacket(m_pasNextAni);
	}
}

void CyborgPlayer::IsRun()
{
		if (m_pasCurrentAni != RUN && m_bIsRun && m_pSkinnedAnimationController->m_fBlendingTime >= 1.0f) {
			SetMaxVelocityXZ(40.f);
			//m_fVelocitySpeed = 20.f;
			m_pasNextAni = RUN;
			m_pSkinnedAnimationController->m_fBlendingTime = 0.0f;
			AnimationPacket(m_pasNextAni);
		}
}

void CyborgPlayer::IsCreep()
{
	if (m_pasCurrentAni != CREEP && m_bIsCreep && !m_bIsRun && m_pSkinnedAnimationController->m_fBlendingTime >= 1.0f) {
		SetMaxVelocityXZ(10.f);
		m_pasNextAni = CREEP;
		m_pSkinnedAnimationController->m_fBlendingTime = 0.0f;
		AnimationPacket(m_pasNextAni);
	}
}

void CyborgPlayer::IsCrawl()
{
	if (m_pasCurrentAni != CRAWL && m_bIsCrawl && !m_bIsRun && m_pSkinnedAnimationController->m_fBlendingTime >= 1.0f) {
		SetVelocity(XMFLOAT3(0.f, 0.f, 0.f));
		m_bIsCreep = false;
		m_bIsRun = false;
		m_pasNextAni = CRAWL;
		m_pSkinnedAnimationController->m_fBlendingTime = 0.0f;
		AnimationPacket(m_pasNextAni);
	}
}

void CyborgPlayer::IsWalk()
{
	if (m_pasCurrentAni != WALK && !m_bIsRun && !m_bIsCreep && m_pSkinnedAnimationController->m_fBlendingTime >= 1.0f) {
		SetMaxVelocityXZ(20.f);
		m_pasNextAni = WALK;
		m_pSkinnedAnimationController->m_fBlendingTime = 0.0f;
		AnimationPacket(m_pasNextAni);
	}
}

void CyborgPlayer::IsJump()
{
}

void CyborgPlayer::SetPlayerData(int type)
{
	switch (type) {
	case 0:
		m_fVelocitySpeed = 20.f;
		m_fMaxStaminer = 100.f;
		m_fStaminer = m_fMaxStaminer;
		m_fRepairSpeed = 50.f;
		m_fTakeOverSpeed = 50.f;
		m_nCharacter = type;
		break;
	case 1:
		m_fVelocitySpeed = 20.f;
		m_fMaxStaminer = 110.f;
		m_fStaminer = m_fMaxStaminer;
		m_fRepairSpeed = 50.f;
		m_fTakeOverSpeed = 80.f;
		m_nCharacter = type;
		break;
	case 2:
		m_fVelocitySpeed = 20.f;
		m_fMaxStaminer = 90.f;
		m_fStaminer = m_fMaxStaminer;
		m_fRepairSpeed = 80.f;
		m_fTakeOverSpeed = 50.f;
		m_nCharacter = type;
		break;
	}
}



void CyborgPlayer::AnimationPacket(const Player_Animation_ST next_anim)
{
	CS_CHANGE_ANIMATION_PACKET packet;
	packet.size = sizeof(packet);
	packet.type = CS_CHANGE_ANIM;
	packet.ani_st = next_anim;
	SetBuffer(&packet, packet.size);
}