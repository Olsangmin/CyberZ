//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Player.h"
#include "Shader.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CPlayer



CPlayer::CPlayer()
{
	m_pCamera = NULL;
	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fMaxVelocityXZ = 0.0f;
	m_fMaxVelocityY = 0.0f;
	m_fFriction = 0.0f;

	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;

	m_pPlayerUpdatedContext = NULL;
	m_pCameraUpdatedContext = NULL;


	p_id = -99;
	bufSize = 0;
}

CPlayer::~CPlayer()
{
	ReleaseShaderVariables();

	if (m_pCamera) delete m_pCamera;
}

void CPlayer::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pCamera) m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CPlayer::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CPlayer::ReleaseShaderVariables()
{
	if (m_pCamera) m_pCamera->ReleaseShaderVariables();
}

void CPlayer::SetBuffer(void* ptr, size_t size)
{
	if (my_id != p_id) return;
	char* p = reinterpret_cast<char*>(ptr);
	memcpy((sendBuffer + bufSize), p, size);
	bufSize += static_cast<int>(size);
}

void CPlayer::SendPacket()
{
	if (my_id != p_id) return;
	if (0 == bufSize)
		return;

	send(c_socket, sendBuffer, bufSize, 0);
	bufSize = 0;

}

void CPlayer::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity) // 2
{
	if (dwDirection)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		if (dwDirection & DIR_FORWARD) {
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3CLook, fDistance);
		}
		if (dwDirection & DIR_BACKWARD) {
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3CLook, -fDistance);
		}
		if (dwDirection & DIR_RIGHT) {
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3CRight, fDistance);
		}
		if (dwDirection & DIR_LEFT) {
			xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3CRight, -fDistance);
		}

#ifdef USE_NETWORK
		CS_MOVE_PACKET packet;
		packet.size = sizeof(packet);
		packet.type = CS_MOVE;
		packet.dir = xmf3Shift;
		packet.yaw = GetYaw();
		SetBuffer(&packet, packet.size);
#endif // USE_NETWORK
		Move(xmf3Shift, bUpdateVelocity);



	}
}

void CPlayer::Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity) // 3
{
	if (bUpdateVelocity)
	{
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else
	{
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift); // 4 Æ÷Áö¼Ç
		m_pCamera->Move(xmf3Shift);
	}

}

void CPlayer::Rotate(float x, float y, float z)
{
	DWORD nCurrentCameraMode = m_pCamera->GetMode();
	if ((nCurrentCameraMode == FIRST_PERSON_CAMERA) || (nCurrentCameraMode == THIRD_PERSON_CAMERA))
	{
		if (x != 0.0f)
		{
			m_fPitch += x;
			if (m_fPitch > +89.0f) { x -= (m_fPitch - 89.0f); m_fPitch = +89.0f; }
			if (m_fPitch < -89.0f) { x -= (m_fPitch + 89.0f); m_fPitch = -89.0f; }
		}
		if (y != 0.0f)
		{
			m_fYaw += y;
			if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
			if (m_fYaw < 0.0f) m_fYaw += 360.0f;
		}
		if (z != 0.0f)
		{
			m_fRoll += z;
			if (m_fRoll > +20.0f) { z -= (m_fRoll - 20.0f); m_fRoll = +20.0f; }
			if (m_fRoll < -20.0f) { z -= (m_fRoll + 20.0f); m_fRoll = -20.0f; }
		}
		m_pCamera->Rotate(x, y, z);
		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}
	else if (nCurrentCameraMode == SPACESHIP_CAMERA)
	{
		m_pCamera->Rotate(x, y, z);
		if (x != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(x));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		}
		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
		if (z != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(z));
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}

	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);

}

void CPlayer::CameraRotate(float x, float y, float z)
{
	DWORD nCurrentCameraMode = m_pCamera->GetMode();
	if ((nCurrentCameraMode == FIRST_PERSON_CAMERA) || (nCurrentCameraMode == THIRD_PERSON_CAMERA))
	{
		if (x != 0.0f)
		{
			m_fCPitch += x;
			if (m_fCPitch > +89.0f) { x -= (m_fCPitch - 89.0f); m_fCPitch = +89.0f; }
			if (m_fCPitch < -89.0f) { x -= (m_fCPitch + 89.0f); m_fCPitch = -89.0f; }
		}
		if (y != 0.0f)
		{
			m_fCYaw += y;
			if (m_fCYaw > 360.0f) m_fCYaw -= 360.0f;
			if (m_fCYaw < 0.0f) m_fCYaw += 360.0f;
		}
		if (z != 0.0f)
		{
			m_fCRoll += z;
			if (m_fCRoll > +20.0f) { z -= (m_fCRoll - 20.0f); m_fCRoll = +20.0f; }
			if (m_fCRoll < -20.0f) { z -= (m_fCRoll + 20.0f); m_fCRoll = -20.0f; }
		}
		m_pCamera->Rotate(x, y, z);
		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3CUp), XMConvertToRadians(y));
			m_xmf3CLook = Vector3::TransformNormal(m_xmf3CLook, xmmtxRotate);
			m_xmf3CRight = Vector3::TransformNormal(m_xmf3CRight, xmmtxRotate);
		}
	}
	else if (nCurrentCameraMode == SPACESHIP_CAMERA)
	{
	}

	m_xmf3CLook = Vector3::Normalize(m_xmf3CLook);
	m_xmf3CRight = Vector3::CrossProduct(m_xmf3CUp, m_xmf3CLook, true);
	m_xmf3CUp = Vector3::CrossProduct(m_xmf3CLook, m_xmf3CRight, true);
}

void CPlayer::Update(float fTimeElapsed)
{

	// Set Length(Vector)

	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, m_xmf3Gravity);
	float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
	//cout << fLength << endl;
	UpdateAcceleration(fLength);
	UpdateGravity(fLength);
	RotateDirection(20.f);
	UpdatePlayerPostion(fTimeElapsed);
	m_bClear ? 
		m_pCamera->ChangeView(false), m_pCamera->SetOffset(XMFLOAT3(40.0f, 23.0f, 20.0f)), UpdateCameraPosition(fTimeElapsed, m_xmf3BossPos): 
		UpdateCameraPosition(fTimeElapsed, m_xmf3Position);
	UpdateFriction(fTimeElapsed);

#ifdef USE_NETWORK
	SendPacket();
#endif

}

void CPlayer::UpdateGravity(float& fLength)
{
	fLength = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);
	if (fLength > m_fMaxVelocityY) m_xmf3Velocity.y *= (m_fMaxVelocityY / fLength);
}

void CPlayer::RotateDirection(float angle)
{
	XMFLOAT3 xmfVel(m_xmf3Velocity.x, 0.0f, m_xmf3Velocity.z);
	if (Vector3::IsZero(xmfVel))xmfVel = m_xmf3Look;

	float fCurrentAngle = Vector3::Angle(Vector3::Normalize(xmfVel), m_xmf3Look);

	if (fCurrentAngle > 0) {

		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(angle));
		XMFLOAT3 xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);

		float fNextAngle = Vector3::Angle(Vector3::Normalize(xmfVel), xmf3Look);

		fCurrentAngle >= fNextAngle ? Rotate(0.0f, (fCurrentAngle-fNextAngle)/1.5, 0.0f) : Rotate(0.0f, -(fNextAngle - fCurrentAngle) / 1.5, 0.0f);
	}
}

void CPlayer::UpdateAcceleration(float& fLength)
{
	if (fLength > m_fMaxVelocityXZ)
	{
		m_xmf3Velocity.x *= (m_fMaxVelocityXZ / fLength);
		m_xmf3Velocity.z *= (m_fMaxVelocityXZ / fLength);
	}
}

void CPlayer::UpdatePlayerPostion(float fTimeElapsed)
{
	XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false);
	m_xmf3NextPos = xmf3Velocity;
	if (!m_bIntersects) {
		m_xmf3ContactNormal = XMFLOAT3(0.f, 0.f, 0.f);
		Move(xmf3Velocity, false);
	}
	else {
		XMFLOAT3 slidingVec = Vector3::XMVectorToFloat3(XMLoadFloat3(&xmf3Velocity) - 
			XMVector3Dot(XMLoadFloat3(&xmf3Velocity), XMLoadFloat3(&m_xmf3ContactNormal)) * XMLoadFloat3(&m_xmf3ContactNormal));
		Move(slidingVec, false);
	}
	if (m_pPlayerUpdatedContext) OnPlayerUpdateCallback(fTimeElapsed);
}

void CPlayer::UpdateCameraPosition(float fTimeElapsed, XMFLOAT3 xmf3Pos)
{
	DWORD nCurrentCameraMode = m_pCamera->GetMode();
	if (nCurrentCameraMode == THIRD_PERSON_CAMERA || nCurrentCameraMode == FIRST_PERSON_CAMERA) 
		m_pCamera->Update(xmf3Pos, fTimeElapsed);
	if (m_pCameraUpdatedContext) OnCameraUpdateCallback(fTimeElapsed);
	if (nCurrentCameraMode == THIRD_PERSON_CAMERA) m_pCamera->SetLookAt(xmf3Pos);
	m_pCamera->RegenerateViewMatrix();

}

void CPlayer::UpdateFriction(float fTimeElapsed)
{
	float fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (m_fFriction * fTimeElapsed);
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));

}

CCamera* CPlayer::OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode)
{
	CCamera* pNewCamera = NULL;
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		pNewCamera = new CFirstPersonCamera(m_pCamera);
		break;
	case THIRD_PERSON_CAMERA:
		pNewCamera = new CThirdPersonCamera(m_pCamera);
		break;
	case SPACESHIP_CAMERA:
		pNewCamera = new CSpaceShipCamera(m_pCamera);
		break;
	}
	if (nCurrentCameraMode == SPACESHIP_CAMERA)
	{
		m_xmf3Right = Vector3::Normalize(XMFLOAT3(m_xmf3Right.x, 0.0f, m_xmf3Right.z));
		m_xmf3Up = Vector3::Normalize(XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_xmf3Look = Vector3::Normalize(XMFLOAT3(m_xmf3Look.x, 0.0f, m_xmf3Look.z));

		m_fPitch = 0.0f;
		m_fRoll = 0.0f;
		m_fYaw = Vector3::Angle(XMFLOAT3(0.0f, 0.0f, 1.0f), m_xmf3Look);
		if (m_xmf3Look.x < 0.0f) m_fYaw = -m_fYaw;
	}
	else if ((nNewCameraMode == SPACESHIP_CAMERA) && m_pCamera)
	{
		m_xmf3Right = m_pCamera->GetRightVector();
		m_xmf3Up = m_pCamera->GetUpVector();
		m_xmf3Look = m_pCamera->GetLookVector();
	}

	if (pNewCamera)
	{
		pNewCamera->SetMode(nNewCameraMode);
		pNewCamera->SetPlayer(this);
	}

	if (m_pCamera) delete[] m_pCamera;

	return(pNewCamera);
}

void CPlayer::OnPrepareRender()
{
	m_xmf4x4ToParent._11 = m_xmf3Right.x; m_xmf4x4ToParent._12 = m_xmf3Right.y; m_xmf4x4ToParent._13 = m_xmf3Right.z;
	m_xmf4x4ToParent._21 = m_xmf3Up.x; m_xmf4x4ToParent._22 = m_xmf3Up.y; m_xmf4x4ToParent._23 = m_xmf3Up.z;
	m_xmf4x4ToParent._31 = m_xmf3Look.x; m_xmf4x4ToParent._32 = m_xmf3Look.y; m_xmf4x4ToParent._33 = m_xmf3Look.z;
	m_xmf4x4ToParent._41 = m_xmf3Position.x; m_xmf4x4ToParent._42 = m_xmf3Position.y; m_xmf4x4ToParent._43 = m_xmf3Position.z;

	m_xmf4x4ToParent = Matrix4x4::Multiply(XMMatrixScaling(m_xmf3Scale.x, m_xmf3Scale.y, m_xmf3Scale.z), m_xmf4x4ToParent);
}

void CPlayer::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{

	DWORD nCameraMode = (pCamera) ? pCamera->GetMode() : 0x00;
	if (nCameraMode == THIRD_PERSON_CAMERA|| nCameraMode == FIRST_PERSON_CAMERA) CGameObject::Render(pd3dCommandList, pCamera);
}

void CPlayer::Release()
{
	CGameObject::Release();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
#define _WITH_DEBUG_CALLBACK_DATA

void CSoundCallbackHandler::HandleCallback(void* pCallbackData, float fTrackPosition)
{
	_TCHAR* pWavName = (_TCHAR*)pCallbackData;
#ifdef _WITH_DEBUG_CALLBACK_DATA
	TCHAR pstrDebug[256] = { 0 };
	_stprintf_s(pstrDebug, 256, _T("%s(%f)\n"), pWavName, fTrackPosition);
	OutputDebugString(pstrDebug);
#endif
#ifdef _WITH_SOUND_RESOURCE
	PlaySound(pWavName, ::ghAppInstance, SND_RESOURCE | SND_ASYNC);
#else
	PlaySound(pWavName, NULL, SND_FILENAME | SND_ASYNC);
#endif
}