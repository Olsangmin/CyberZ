#pragma once

#include "GUI.h"

class CStartSceneUI : public CUI
{
public:
	CStartSceneUI() {}
	~CStartSceneUI() {}

public:
	void DrawUI(UINT m_nSwapChainBufferIndex);
	void UISet(UINT m_nSwapChainBufferIndex);


};