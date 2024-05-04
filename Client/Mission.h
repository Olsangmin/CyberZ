#pragma once

#include "Object.h"

class Mission :public CGameObject
{
public:
	Mission(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel);;
	virtual ~Mission();

private:
	bool							m_bUnable{ false };

public:
	void SetUnable(bool bUnable) { m_bUnable = bUnable; }
	bool GetUnable() { return m_bUnable; }
};

