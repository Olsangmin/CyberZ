#pragma once
#include "Object.h"

class CRobotObject : public CGameObject
{
public:
	CRobotObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel, int nAnimationTracks);
	virtual ~CRobotObject();

	virtual void Update(float fTimeElapsed);

	void AnimationBlending(Player_Animation_ST type1, Player_Animation_ST type2);

	virtual void IsMove(Player_Animation_ST CheckAni);
	virtual void IsIdle();
	virtual void IsAttack();

	Player_Animation_ST m_pasCurrentAni;
	Player_Animation_ST m_pasNextAni;
};