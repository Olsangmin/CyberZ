
#include "stdafx.h"
#include "TagButton.h"

CTagButton::CTagButton(float x, float y, float width, float height)
{
	m_fLeft = x;
	m_fRight = x + width;
	m_fTop = y;
	m_fBottom = y + height;

	m_rButtonRect = { m_fLeft , m_fTop, m_fRight, m_fBottom };
}

CTagButton::CTagButton(D2D1_RECT_F rect)
{
	m_fLeft = rect.left;
	m_fRight = rect.right;
	m_fTop = rect.top;
	m_fBottom = rect.bottom;

	m_rButtonRect = rect;
}

void CTagButton::SetPosition(float x, float y)
{
	m_fLeft = x;
	m_fTop = y;
}

void CTagButton::SetSize(float width, float height)
{
	m_fWidth = width;
	m_fHeight = height;
	m_fRight = m_fLeft + width;
	m_fBottom = m_fTop + height;
}

void CTagButton::Draw(ID2D1DeviceContext2* pd2dDeviceContext, ID2D1SolidColorBrush* pd2dbrBorder, int pUISphare)
{

	m_fRight = m_fLeft + m_fWidth;
	m_fBottom = m_fTop + m_fHeight;

	m_rButtonRect = { m_fLeft, m_fTop, m_fRight, m_fBottom };
	if (pUISphare == 0) 
	{
		pd2dbrBorder->SetColor(customColor);
		pd2dDeviceContext->FillRectangle(m_rButtonRect, pd2dbrBorder);
	}

}

bool CTagButton::CheckMouseOn(HWND hWnd, POINT CursorPos)
{
	m_bMouseOn = false;

	// 마우스 커서 위치 계산
	RECT WindowRect;
	GetWindowRect(hWnd, &WindowRect);

	POINT mousePos;
#ifdef FULL_SCREEN

	mousePos.x = CursorPos.x - WindowRect.left;
	mousePos.y = CursorPos.y - WindowRect.top;

#else

	mousePos.x = CursorPos.x - WindowRect.left;
	mousePos.y = CursorPos.y - WindowRect.top - 30;

#endif // FULL_SCREEN

	// 충돌확인
	if (mousePos.x > m_fLeft && mousePos.x < m_fRight)
	{
		if (mousePos.y > m_fTop && mousePos.y < m_fBottom)
		{ 
			m_bMouseOn = true;
		}
	}

	return(m_bMouseOn);
}
