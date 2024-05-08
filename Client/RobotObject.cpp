#include "RobotObject.h"
#include "stdafx.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CRobotObject::CRobotObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel, int nAnimationTracks)
{
	CLoadedModelInfo* pRobotModel = pModel;
	//if (!pRobotModel) pRobotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Robot.bin", NULL);

	SetChild(pRobotModel->m_pModelRootObject, true);

	m_pSkinnedAnimationController = new CAnimationController(pd3dDevice, pd3dCommandList, nAnimationTracks, pRobotModel);

	for (int i = 0; i < 3; ++i)
		m_pSkinnedAnimationController->SetTrackAnimationSet(i, i);

	// Default animation setting
	m_pSkinnedAnimationController->SetAllTrackDisable();
	m_pSkinnedAnimationController->SetTrackEnable(0, true);
	m_pSkinnedAnimationController->SetTrackSpeed(0, 0.5f);
}

CRobotObject::~CRobotObject()
{
}

void CRobotObject::Update(float fTimeElapsed)
{
	CGameObject::Update(fTimeElapsed);
	if (m_pSkinnedAnimationController) {
		AnimationBlending(m_pasCurrentAni, m_pasNextAni);
		IsMove(m_pasNextAni);

	}
}

void CRobotObject::AnimationBlending(Player_Animation_ST type1, Player_Animation_ST type2)
{
	if (m_pSkinnedAnimationController->m_fBlendingTime <= 1.0f && type2 != NONE) {
		// If need to blending
		// 체인지 애니메이션(섞인거) 
		m_pSkinnedAnimationController->m_fBlendingWeight = 2.0f;
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

void CRobotObject::IsMove(Player_Animation_ST CheckAni)
{
	if (m_pasCurrentAni != CheckAni && m_pSkinnedAnimationController->m_fBlendingTime >= 1.0f) {
		m_pasNextAni = CheckAni;
		m_pSkinnedAnimationController->m_fBlendingTime = 0.0f;
		m_pSkinnedAnimationController->SetTrackSpeed(0, 0.3f);
		// AnimationPacket(m_pasNextAni);
	}
}

void CRobotObject::IsIdle()
{
}

void CRobotObject::IsAttack()
{
}