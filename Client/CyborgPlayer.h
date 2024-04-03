#pragma once
#include "Player.h"

class CyborgPlayer : public CPlayer
{
public:
	CyborgPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL, CLoadedModelInfo* pModel = NULL);
	virtual ~CyborgPlayer();

	Player_Animation_ST GetCurrentAni() { return m_pasCurrentAni; }
	Player_Animation_ST GetNextAni() { return m_pasNextAni; }

public:
	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);

	virtual void OnPlayerUpdateCallback(float fTimeElapsed);
	virtual void OnCameraUpdateCallback(float fTimeElapsed);

	virtual void Move(DWORD dwDirection, float fDistance, bool bVelocity = false);

	virtual void Update(float fTimeElapsed);

	void AnimationBlending(Player_Animation_ST type1, Player_Animation_ST type2);

	Player_Animation_ST m_pasCurrentAni;
	Player_Animation_ST m_pasNextAni;

	bool m_bIsRun{ false };
	bool m_bIsCreep{ false };
	bool m_bIsCreep_flag{ false };
	bool m_bIsJump{ false };

	//-------------------------------------
	void AnimationPacket(const Player_Animation_ST next_anim);
	//-------------------------------------
};

