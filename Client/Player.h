#pragma once

#define DIR_FORWARD				0x01
#define DIR_BACKWARD			0x02
#define DIR_LEFT				0x04
#define DIR_RIGHT				0x08
#define DIR_UP					0x10
#define DIR_DOWN				0x20

#include "Mission.h"
#include "Camera.h"

class CPlayer : public CGameObject
{
protected:
	XMFLOAT3					m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3					m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	XMFLOAT3					m_xmf3NextPos = XMFLOAT3(0.f, 0.f, 0.f);

	XMFLOAT3					m_xmf3CRight = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3CUp = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3					m_xmf3CLook = XMFLOAT3(0.0f, 0.0f, 1.0f);

	float           			m_fCPitch = 0.0f;
	float           			m_fCYaw = 0.0f;
	float           			m_fCRoll = 0.0f;

	XMFLOAT3					m_xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	float           			m_fPitch = 0.0f;
	float           			m_fYaw = 0.0f;
	float           			m_fRoll = 0.0f;

	XMFLOAT3					m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	XMFLOAT3     				m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float           			m_fMaxVelocityXZ = 0.0f;
	float           			m_fMaxVelocityY = 0.0f;
	float           			m_fFriction = 0.0f;

	LPVOID						m_pPlayerUpdatedContext = NULL;
	LPVOID						m_pCameraUpdatedContext = NULL;


public:
	bool						m_bIntersects = false;
	XMFLOAT3					m_xmf3ContactNormal{ 0.f, 0.f, 0.f };
	CCamera						*m_pCamera = NULL;
	bool						m_bUnable = false;
	bool						m_bReady = false;
	bool						m_bSliding = false;
	bool						m_bClear = false;
	XMFLOAT3					m_xmf3BossPos{};

	XMFLOAT3					m_xmf3BeforeColliedPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);

	// @@서버코드@@서버코드@@서버코드@@서버코드@@서버코드@@서버코드@@
	int							p_id;
	char						sendBuffer[1024];
	int							bufSize;
	void						SetBuffer(void* ptr, size_t size);
	void						SendPacket();
	// @@서버코드@@서버코드@@서버코드@@서버코드@@서버코드@@서버코드@@

public:
	CPlayer();
	virtual ~CPlayer();

	XMFLOAT3 GetPosition() { return(m_xmf3Position); }
	XMFLOAT3 GetLookVector() { return(m_xmf3Look); }
	XMFLOAT3 GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3 GetRightVector() { return(m_xmf3Right); }

	XMFLOAT3 GetCLookVector() { return(m_xmf3CLook); }
	XMFLOAT3 GetCUpVector() { return(m_xmf3CUp); }
	XMFLOAT3 GetCRightVector() { return(m_xmf3CRight); }
	XMFLOAT3 GetCVelocityVector() { return(m_xmf3Velocity); }
	

	void SetFriction(float fFriction) { m_fFriction = fFriction; }
	void SetGravity(const XMFLOAT3& xmf3Gravity) { m_xmf3Gravity = xmf3Gravity; }
	void SetMaxVelocityXZ(float fMaxVelocity) { m_fMaxVelocityXZ = fMaxVelocity; }
	void SetMaxVelocityY(float fMaxVelocity) { m_fMaxVelocityY = fMaxVelocity; }
	void SetVelocity(const XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }
	void SetPosition(const XMFLOAT3& xmf3Position) { Move(XMFLOAT3(xmf3Position.x - m_xmf3Position.x, xmf3Position.y - m_xmf3Position.y, xmf3Position.z - m_xmf3Position.z), false); }

	void SetScale(XMFLOAT3& xmf3Scale) { m_xmf3Scale = xmf3Scale; }

	const XMFLOAT3& GetVelocity() const { return(m_xmf3Velocity); }
	float GetYaw() const { return(m_fYaw); }
	float GetPitch() const { return(m_fPitch); }
	float GetRoll() const { return(m_fRoll); }

	CCamera *GetCamera() { return(m_pCamera); }
	void SetCamera(CCamera *pCamera) { m_pCamera = pCamera; }

	virtual void Move(DWORD nDirection, float fDistance, bool bVelocity = false);
	virtual void Move(DWORD dwDirection, DWORD dwLastDirection, float fDistance, bool bVelocity = false) {};
	void Move(const XMFLOAT3& xmf3Shift, bool bVelocity = false);
	void Move(float fxOffset = 0.0f, float fyOffset = 0.0f, float fzOffset = 0.0f);
	void Rotate(float x, float y, float z);
	void CameraRotate(float x, float y, float z);
	void RotateDirection(float angle);

	virtual void Update(float fTimeElapsed);
	void UpdateGravity(float& fLength);
	void UpdateAcceleration(float& fLength);
	void UpdatePlayerPostion(float fTimeElapsed);
	void UpdateCameraPosition(float fTimeElapsed, XMFLOAT3 xmf3Pos);
	void UpdateFriction(float fTimeElapsed);

	virtual void OnPlayerUpdateCallback(float fTimeElapsed) { }
	void SetPlayerUpdatedContext(LPVOID pContext) { m_pPlayerUpdatedContext = pContext; }

	virtual void OnCameraUpdateCallback(float fTimeElapsed) { }
	void SetCameraUpdatedContext(LPVOID pContext) { m_pCameraUpdatedContext = pContext; }

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);

	CCamera *OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode);

	virtual CCamera *ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed) { return(NULL); }
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
	virtual void Release();

	// Animation
	virtual void SetRun(bool value) {};
	virtual void SetCreep() {};
	virtual void SetCreepFlag() {};
	virtual void SetJump() {};

	virtual void IsRun() {};
	virtual void IsCreep() {};
	virtual void IsCrawl() {};
	virtual void IsWalk() {};
	virtual void IsJump() {};

	// Data
	virtual void SetPlayerData(int type) {};

	virtual bool GetStaminer() { return 0; };
	virtual float GetVelocitySpeed() { return 0; };

};


class CSoundCallbackHandler : public CAnimationCallbackHandler
{
public:
	CSoundCallbackHandler() { }
	~CSoundCallbackHandler() { }

public:
	virtual void HandleCallback(void *pCallbackData, float fTrackPosition); 
};