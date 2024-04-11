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

public:

	Player_Animation_ST m_pasCurrentAni;
	Player_Animation_ST m_pasNextAni;

	float m_fVelocitySpeed{};
	float m_fStaminer{};
	float m_fRepairSpeed{};
	float m_fTakeOverSpeed{};
	
	int	m_nCharacter;

	bool m_bIsRun{ false };
	bool m_bIsCreep{ false };
	bool m_bIsCreep_flag{ false };
	bool m_bIsJump{ false };

	bool m_bIsStaminer{ true };
	virtual float GetStaminer() { return m_fStaminer; };

	// Animation
	virtual void SetRun(bool value);
	virtual void SetCreep();
	virtual void SetCreepFlag();
	virtual void SetJump();

	virtual void IsIdle();
	virtual void IsRun();
	virtual void IsCreep();
	virtual void IsCrawl();
	virtual void IsWalk();
	virtual void IsJump();

	// Data
	virtual void SetPlayerData(int type);

	//-------------------------------------
	void AnimationPacket(const Player_Animation_ST next_anim);
	//-------------------------------------
};

