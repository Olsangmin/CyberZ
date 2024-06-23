
#include "stdafx.h"
#include "StartScene.h"

void CStartScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int myPlayernum)
{
	CScene::BuildObjects(pd3dDevice, pd3dCommandList, myPlayernum);


	m_pUI = new CStartSceneUI();

}

void CStartScene::ProcessPacket(char* p)
{

}
