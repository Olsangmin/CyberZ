//------------------------------------------------------- ----------------------
// File: Object.h
//-----------------------------------------------------------------------------

#pragma once

// #include "Mesh.h"
#include "Camera.h"
#include "Animation.h"

#define DIR_FORWARD					0x01
#define DIR_BACKWARD				0x02
#define DIR_LEFT					0x04
#define DIR_RIGHT					0x08
#define DIR_UP						0x10
#define DIR_DOWN					0x20

class CShader;
class CStandardShader;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define RESOURCE_TEXTURE2D			0x01
#define RESOURCE_TEXTURE2D_ARRAY	0x02	//[]
#define RESOURCE_TEXTURE2DARRAY		0x03
#define RESOURCE_TEXTURE_CUBE		0x04
#define RESOURCE_BUFFER				0x05

class CTexture
{
public:
	CTexture(int nTextureResources, UINT nResourceType, int nSamplers, int nRootParameters);
	virtual ~CTexture();

private:

	int								m_nReferences = 0;
	UINT							m_nTextureType;

	int								m_nTextures = 0;
	ID3D12Resource** m_ppd3dTextures = NULL;
	ID3D12Resource** m_ppd3dTextureUploadBuffers;

	UINT* m_pnResourceTypes = NULL;

	DXGI_FORMAT* m_pdxgiBufferFormats = NULL;
	int* m_pnBufferElements = NULL;

	int								m_nRootParameters = 0;
	int								*m_pnRootParameterIndices = NULL;
	D3D12_GPU_DESCRIPTOR_HANDLE		*m_pd3dSrvGpuDescriptorHandles = NULL;

	int								m_nSamplers = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE* m_pd3dSamplerGpuDescriptorHandles = NULL;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle);

	void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int nParameterIndex, int nTextureIndex);
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseShaderVariables();

	// void LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, UINT nIndex);
	void LoadTextureFromDDSFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, UINT nResourceType, UINT nIndex);
	// void LoadBufferFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, wchar_t *pszFileName, UINT nIndex);
	void LoadBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nElements, UINT nStride, DXGI_FORMAT ndxgiFormat, UINT nIndex);
	ID3D12Resource* CreateTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nIndex, UINT nResourceType, UINT nWidth, UINT nHeight, UINT nElements, UINT nMipLevels, DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS d3dResourceFlags, D3D12_RESOURCE_STATES d3dResourceStates, D3D12_CLEAR_VALUE* pd3dClearValue);

	void SetRootParameterIndex(int nIndex, UINT nRootParameterIndex);
	void SetGpuDescriptorHandle(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle);

	int GetRootParameters() { return(m_nRootParameters); }
	int GetTextures() { return(m_nTextures); }
	ID3D12Resource* GetResource(int nIndex) { return(m_ppd3dTextures[nIndex]); }

	UINT GetTextureType() { return(m_nTextureType); }
	UINT GetTextureType(int nIndex) { return(m_pnResourceTypes[nIndex]); }
	DXGI_FORMAT GetBufferFormat(int nIndex) { return(m_pdxgiBufferFormats[nIndex]); }
	int GetBufferElements(int nIndex) { return(m_pnBufferElements[nIndex]); }

	D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(int nIndex);

	void ReleaseUploadBuffers();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define MATERIAL_ALBEDO_MAP				0x01
#define MATERIAL_SPECULAR_MAP			0x02
#define MATERIAL_NORMAL_MAP				0x04
#define MATERIAL_METALLIC_MAP			0x08
#define MATERIAL_EMISSION_MAP			0x10
#define MATERIAL_DETAIL_ALBEDO_MAP		0x20
#define MATERIAL_DETAIL_NORMAL_MAP		0x40

class CGameObject;

class CMaterial
{
public:
	CMaterial(int nTextures);
	virtual ~CMaterial();

private:
	int								m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

public:
	CShader							*m_pShader = NULL;

	XMFLOAT4						m_xmf4AlbedoColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4						m_xmf4EmissiveColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4						m_xmf4SpecularColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4						m_xmf4AmbientColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	void SetShader(CShader *pShader);
	void SetMaterialType(UINT nType) { m_nType |= nType; }
	void SetTexture(CTexture *pTexture, UINT nTexture = 0);

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void ReleaseUploadBuffers();

public:
	UINT							m_nType = 0x00;

	float							m_fGlossiness = 0.0f;
	float							m_fSmoothness = 0.0f;
	float							m_fSpecularHighlight = 0.0f;
	float							m_fMetallic = 0.0f;
	float							m_fGlossyReflection = 0.0f;

public:
	int 							m_nTextures = 0;
	_TCHAR							(*m_ppstrTextureNames)[64] = NULL;
	CTexture						**m_ppTextures = NULL; //0:Albedo, 1:Specular, 2:Metallic, 3:Normal, 4:Emission, 5:DetailAlbedo, 6:DetailNormal

	void LoadTextureFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, UINT nType, UINT nRootParameter, _TCHAR *pwstrTextureName, CTexture **ppTexture, CGameObject *pParent, FILE *pInFile, CShader *pShader);

public:
	static CShader					*m_pStandardShader;
	static CShader					*m_pSkinnedAnimationShader;
	static CShader					*m_pBoundingBoxShader;
	static CShader					*m_pParticleShader;


	static void CMaterial::PrepareShaders(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);

	void SetStandardShader() { CMaterial::SetShader(m_pStandardShader); }
	void SetSkinnedAnimationShader() { CMaterial::SetShader(m_pSkinnedAnimationShader); }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CGameObject
{
private:
	int								m_nReferences = 0;

public:
	void AddRef();
	virtual void Release();

public:
	CGameObject();
	CGameObject(int nMaterials);
    virtual ~CGameObject();

public:
	char							m_pstrFrameName[64];

	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorHandle;

	CMesh							*m_pMesh = NULL;

	int								m_nMaterials = 0;
	CMaterial						**m_ppMaterials = NULL;
	
	XMFLOAT4X4						m_xmf4x4ToParent;
	XMFLOAT4X4						m_xmf4x4World;

	CGameObject 					*m_pParent = NULL;
	CGameObject 					*m_pChild = NULL;
	CGameObject 					*m_pSibling = NULL;

	CAnimationController*			m_pSkinnedAnimationController = NULL;


	BoundingOrientedBox				m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.1f, 0.1f, 0.1f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)); CBoundingBoxMesh				*m_pBoundingBoxMesh = NULL;
	bool							m_bCheckBB = false;
	
	void SetBoundingBoxMesh(CBoundingBoxMesh* pMesh);
	void UpdateBoundingBox(XMFLOAT3 xmf3NextPos);
	virtual void RenderBoundingBox(ID3D12GraphicsCommandList* pd3dCommandList);
	void MoveBBToParent(CGameObject* pTargetLv);


	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle) { m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorHandle; }
	void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr) { m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle() { return(m_d3dCbvGPUDescriptorHandle); }


	void SetMesh(CMesh *pMesh);
	void SetShader(CShader *pShader);
	void SetShader(int nMaterial, CShader *pShader);
	void SetMaterial(int nMaterial, CMaterial *pMaterial);

	void SetChild(CGameObject *pChild, bool bReferenceUpdate=false);

	virtual void BuildMaterials(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) { }

	virtual void OnPrepareAnimate() { }
	virtual void Animate(float fTimeElapsed);

	virtual void Update(float fTimeElapsed) {};

	virtual void OnPrepareRender() { }
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera=NULL);

	virtual void OnLateUpdate() { }

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4 *pxmf4x4World);
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, CMaterial *pMaterial);

	virtual void ReleaseUploadBuffers();

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	XMFLOAT3 GetToParentPosition();
	void Move(XMFLOAT3 xmf3Offset);

	virtual void SetPosition(float x, float y, float z);
	virtual void SetPosition(XMFLOAT3 xmf3Position);
	void SetScale(float x, float y, float z);

	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3 *pxmf3Axis, float fAngle);
	void Rotate(XMFLOAT4 *pxmf4Quaternion);

	// Animation
	virtual void IsMove() {};
	virtual void IsIdle() {};
	virtual void IsAttack() {};

	CGameObject *GetParent() { return(m_pParent); }
	void UpdateTransform(XMFLOAT4X4 *pxmf4x4Parent=NULL);
	CGameObject *FindFrame(char *pstrFrameName);

	CTexture *FindReplicatedTexture(_TCHAR *pstrTextureName);

	UINT GetMeshType() { return((m_pMesh) ? m_pMesh->GetType() : 0x00); }

public:
	CSkinnedMesh *FindSkinnedMesh(char *pstrSkinnedMeshName);
	void FindAndSetSkinnedMesh(CSkinnedMesh **ppSkinnedMeshes, int *pnSkinnedMesh);

	void SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet);
	void SetTrackAnimationPosition(int nAnimationTrack, float fPosition);

	void SetRootMotion(bool bRootMotion) { if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->SetRootMotion(bRootMotion); }

	void LoadMaterialsFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CGameObject *pParent, FILE *pInFile, CShader *pShader);

	static void LoadAnimationFromFile(FILE *pInFile, CLoadedModelInfo *pLoadedModel);
	static CGameObject *LoadFrameHierarchyFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, CGameObject *pParent, FILE *pInFile, CShader *pShader, int *pnSkinnedMeshes);
	static CLoadedModelInfo *LoadGeometryAndAnimationFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, char *pstrFileName, CShader *pShader);

	static void PrintFrameInfo(CGameObject *pGameObject, CGameObject *pParent);

	bool IsVisible(CCamera* pCamera = NULL);

};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color);
	virtual ~CHeightMapTerrain();

private:
	CHeightMapImage				*m_pHeightMapImage;

	int							m_nWidth;
	int							m_nLength;

	XMFLOAT3					m_xmf3Scale;

public:
	float GetHeight(float x, float z, bool bReverseQuad = false) { return(m_pHeightMapImage->GetHeight(x, z, bReverseQuad) * m_xmf3Scale.y); } //World
	XMFLOAT3 GetNormal(float x, float z) { return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z / m_xmf3Scale.z))); }

	int GetHeightMapWidth() { return(m_pHeightMapImage->GetHeightMapWidth()); }
	int GetHeightMapLength() { return(m_pHeightMapImage->GetHeightMapLength()); }

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	float GetLength() { return(m_nLength * m_xmf3Scale.z); }
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CSkyBox : public CGameObject
{
public:
	CSkyBox(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, wchar_t* pszFileName);
	virtual ~CSkyBox();

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CParticle : public CGameObject
{
public:
	CParticle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, XMFLOAT3 xmf3Pos);
	virtual ~CParticle();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CAngrybotAnimationController : public CAnimationController
{
public:
	CAngrybotAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, CLoadedModelInfo* pModel);
	~CAngrybotAnimationController();

	virtual void OnRootMotion(CGameObject* pRootGameObject);
};

class CAngrybotObject : public CGameObject
{
public:
	CAngrybotObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, CLoadedModelInfo *pModel, int nAnimationTracks);
	virtual ~CAngrybotObject();
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CRootMotionCallbackHandler : public CAnimationCallbackHandler
{
public:
	CRootMotionCallbackHandler() { }
	~CRootMotionCallbackHandler() { }

public:
	virtual void HandleCallback(void* pCallbackData, float fTrackPosition);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

class CRobotObject : public CGameObject
{
private:

	XMFLOAT3				m_xmf3Target{ 0.0f, 0.0f, 0.0f };
	bool					m_bAttackStatus = false;

public:
	CRobotObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, CLoadedModelInfo *pModel, int nAnimationTracks);
	virtual ~CRobotObject();

	virtual void Update(float fTimeElapsed);

	void SetAttackStatus(bool status) { m_bAttackStatus = status; }
	void MoveToTarget();
	bool RotateDirection(float fAngle, XMFLOAT3& xmf3Target);

	void AnimationBlending(Player_Animation_ST type1, Player_Animation_ST type2);

	void SetTarget(XMFLOAT3 xmf3Target) { m_xmf3Target = xmf3Target; }
	XMFLOAT3 GetTarget() { return m_xmf3Target; }
	virtual void IsMove(Player_Animation_ST CheckAni);
	virtual void IsIdle();
	bool IsAttackP();

	Player_Animation_ST m_pasCurrentAni;
	Player_Animation_ST m_pasNextAni;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CBossRobotObject : public CGameObject
{
private:

	XMFLOAT3				m_xmf3Target{ 0.0f, 0.0f, 0.0f };
	bool					m_bAttackStatus = false;
	bool					m_bDiyingStatus = false;

	int						m_nDiyCheck = 0;

public:
	CBossRobotObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel, int nAnimationTracks);
	virtual ~CBossRobotObject();

	virtual void Update(float fTimeElapsed);


	void SetDiying(bool IsDie) { m_bDiyingStatus = IsDie; }
	void SetAttackStatus(bool status, int _AttackType) { m_bAttackStatus = status;  AttackType = _AttackType;
	}
	void MoveToTarget();
	void RotateDirection(float fAngle);

	void AnimationBlending(Player_Animation_ST type1, Player_Animation_ST type2);

	void SetTarget(XMFLOAT3 xmf3Target) { m_xmf3Target = xmf3Target; }
	XMFLOAT3 GetTarget() { return m_xmf3Target; }
	virtual void IsMove(Player_Animation_ST CheckAni);
	bool IsAttackP(Player_Animation_ST Status);
	int IsDiying();

	int GetDiy() { return m_nDiyCheck; }

	Player_Animation_ST m_pasCurrentAni;
	Player_Animation_ST m_pasNextAni;
	int AttackType{};
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CStandardOBJ : public CGameObject
{
public:
	CStandardOBJ() {};
	CStandardOBJ(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel);
	virtual ~CStandardOBJ();
};

class CMissonOBJ : public CGameObject
{
public:
	CMissonOBJ(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel, XMFLOAT3 f3MissionRange, int nCategory);
	virtual ~CMissonOBJ();

	CBoundingBoxMesh*				m_pMissionRangeMesh = NULL;
	BoundingOrientedBox				m_xmMissionRange = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.1f, 0.1f, 0.1f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	
	// 미션 종류
	// 0 - 점령미션, 1 - 보안키 미션
	int		m_nCategory = 0;
	bool	m_bMissionflag = false;

	bool	m_bEnd = false;

public:
	void RenderBoundingBox(ID3D12GraphicsCommandList* pd3dCommandList);

	void SetMissionRange(float x, float y, float z);
	void SetMissionRangeMesh(CBoundingBoxMesh* pMesh);
	void RenderMissionRange(ID3D12GraphicsCommandList* pd3dCommandList);

	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);


	void SetMissionCategory(int n) { m_nCategory = n; }
};


class CFloorObj : public CGameObject
{
public: 
	CFloorObj(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel);
	virtual ~CFloorObj();

	BoundingOrientedBox				m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.1f, 0.1f, 0.1f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	
	CBoundingBoxMesh* m_pMissionRangeMesh = NULL;

};

