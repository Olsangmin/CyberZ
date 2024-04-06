#include "stdafx.h"
#include "CyborgPlayer.h"

CyborgPlayer::CyborgPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext, CLoadedModelInfo* pModel)
{
	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);

	CLoadedModelInfo* pPlayerModel = pModel;

	SetChild(pPlayerModel->m_pModelRootObject, true);

	m_pSkinnedAnimationController = new CAnimationController(pd3dDevice, pd3dCommandList, MAX_ANIMATION_TYPE, pPlayerModel);

	// Animation Setting
	for (int i = 0; i < MAX_ANIMATION_TYPE; ++i)
		m_pSkinnedAnimationController->SetTrackAnimationSet(i, i);

	// Default animation setting
	m_pSkinnedAnimationController->SetAllTrackDisable();
	m_pSkinnedAnimationController->SetTrackEnable(IDLE, true);
	m_pasCurrentAni = IDLE;

	// Sound setting
	m_pSkinnedAnimationController->SetCallbackKeys(1, 2);
#ifdef _WITH_SOUND_RESOURCE
	m_pSkinnedAnimationController->SetCallbackKey(0, 0.1f, _T("Footstep01"));
	m_pSkinnedAnimationController->SetCallbackKey(1, 0.5f, _T("Footstep02"));
	m_pSkinnedAnimationController->SetCallbackKey(2, 0.9f, _T("Footstep03"));
#else
	m_pSkinnedAnimationController->SetCallbackKey(1, 0, 0.2f, _T("Sound/Footstep01.wav"));
	m_pSkinnedAnimationController->SetCallbackKey(1, 1, 0.5f, _T("Sound/Footstep02.wav"));
	//	m_pSkinnedAnimationController->SetCallbackKey(1, 2, 0.39f, _T("Sound/Footstep03.wav"));
#endif
	CAnimationCallbackHandler* pAnimationCallbackHandler = new CSoundCallbackHandler();
	m_pSkinnedAnimationController->SetAnimationCallbackHandler(1, pAnimationCallbackHandler);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	SetPlayerUpdatedContext(pContext);
	SetCameraUpdatedContext(pContext);

	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;
	SetPosition(XMFLOAT3(0.0f, pTerrain->GetHeight(310.0f, 590.0f), 0.0f));
	SetScale(XMFLOAT3(10.0f, 10.0f, 10.0f));

	if (pPlayerModel) delete pPlayerModel;
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
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, 0.0f));
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
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 200.0f, 0.0f));
		//m_pCamera->Rotate(90.f, 0.f, 0.f);
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case THIRD_PERSON_CAMERA:
		SetFriction(130.0f);
		SetGravity(XMFLOAT3(0.0f, -250.0f, 0.0f));
		SetMaxVelocityXZ(40.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.05f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, -40.0f));
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
		//AnimationBlending(m_pasCurrentAni, WALK);
		/*if (m_pasCurrentAni != WALK && !m_bIsRun && !m_bIsCreep && m_pSkinnedAnimationController->m_fBlendingTime >= 1.0f) {
			m_pasNextAni = WALK;
			m_pSkinnedAnimationController->m_fBlendingTime = 0.0f;
			AnimationPacket(m_pasNextAni);
		}*/
		IsWalk();
	}

	CPlayer::Move(dwDirection, fDistance, bUpdateVelocity);
}

void CyborgPlayer::Update(float fTimeElapsed)
{
	CPlayer::Update(fTimeElapsed);


	if (m_pSkinnedAnimationController)
	{
		AnimationBlending(m_pasCurrentAni, m_pasNextAni);
		ExhaustionStaminer();
		RestorationStaminer();
		IsRun();
		IsCreep();
		float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
		if (::IsZero(fLength))
		{
			IsIdle();
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

void CyborgPlayer::ExhaustionStaminer()
{
	if (m_bIsRun)
		if(m_fStaminer>0)m_fStaminer-=0.2f;
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
			SetMaxVelocityXZ(80.f);
			m_pasNextAni = RUN;
			m_pSkinnedAnimationController->m_fBlendingTime = 0.0f;
			AnimationPacket(m_pasNextAni);
		}
}

void CyborgPlayer::IsCreep()
{
	if (m_pasCurrentAni != CRAWL && m_bIsCreep && !m_bIsRun && m_pSkinnedAnimationController->m_fBlendingTime >= 1.0f) {
		SetMaxVelocityXZ(20.f);
		m_pasNextAni = CRAWL;
		m_pSkinnedAnimationController->m_fBlendingTime = 0.0f;
		AnimationPacket(m_pasNextAni);
	}
}

void CyborgPlayer::IsCrawl()
{
}

void CyborgPlayer::IsWalk()
{
	if (m_pasCurrentAni != WALK && !m_bIsRun && !m_bIsCreep && m_pSkinnedAnimationController->m_fBlendingTime >= 1.0f) {
		SetMaxVelocityXZ(40.f);
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
		m_fVelocitySpeed = 4.5f;
		m_fMaxStaminer = 100.f;
		m_fStaminer = m_fMaxStaminer;
		m_fRepairSpeed = 50.f;
		m_fTakeOverSpeed = 50.f;
		break;
	case 1:
		m_fVelocitySpeed = 4.5f;
		m_fMaxStaminer = 110.f;
		m_fStaminer = m_fMaxStaminer;
		m_fRepairSpeed = 50.f;
		m_fTakeOverSpeed = 80.f;
		break;
	case 2:
		m_fVelocitySpeed = 4.5f;
		m_fMaxStaminer = 90.f;
		m_fStaminer = m_fMaxStaminer;
		m_fRepairSpeed = 80.f;
		m_fTakeOverSpeed = 50.f;
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