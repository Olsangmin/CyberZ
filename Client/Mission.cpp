#include "stdafx.h"
#include "Mission.h"


Mission::Mission(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel)
{
	CLoadedModelInfo* pObjectModel = pModel;
	if (!pObjectModel) pObjectModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/BigContainer.bin", NULL);

	SetChild(pObjectModel->m_pModelRootObject, true);
}

Mission::~Mission()
{
}
