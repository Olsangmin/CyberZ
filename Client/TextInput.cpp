#include "stdafx.h"
#include "TextInput.h"

CTextInput::CTextInput(float x, float y, float width, float height, const wchar_t* text)
{

	m_fLeft = x;
	m_fRight = x + width;
	m_fTop = y;
	m_fBottom = y + height;

	m_rTextInput = { m_fLeft , m_fTop, m_fRight, m_fBottom };
	m_rButtonBox = { m_fLeft, m_fTop, m_fRight, m_fBottom };

	m_text = text;


}

void CTextInput::Release()
{
	if (m_pTextFormat) m_pTextFormat->Release();
}

void CTextInput::SetSize(float width, float height)
{ 
	m_fWidth = width; 
	m_fHeight = height; 
	
	m_fRight = m_fLeft + m_fWidth;
	m_fBottom = m_fTop + m_fHeight;
}

void CTextInput::CreateTextFormat(IDWriteFactory* pDWriteFactory, float fontSize)
{
	if (!m_pTextFormat)
	{
		pDWriteFactory->CreateTextFormat(
			L"Arial",                // Font family name
			NULL,                    // Font collection (NULL sets it to use the system font collection)
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			fontSize,
			L"en-us",
			&m_pTextFormat
		);
	}
}

void CTextInput::Draw(ID2D1DeviceContext2* pd2dDeviceContext, ID2D1SolidColorBrush* pd2dbrText, ID2D1SolidColorBrush* pd2dbrBorder)
{
	m_rTextInput = { m_fLeft+5, m_fTop, m_fRight, m_fBottom };
	m_rButtonBox = {m_fLeft, m_fTop, m_fRight, m_fBottom };
	
	//Box
	if (m_bSelected) pd2dbrBorder->SetColor(D2D1::ColorF(D2D1::ColorF::DarkGray, 0.9f));
	else pd2dbrBorder->SetColor(D2D1::ColorF(D2D1::ColorF::White, 0.9f));
	
	pd2dDeviceContext->FillRectangle(m_rButtonBox, pd2dbrBorder);


	//text
	if (m_pTextFormat)
	{
		//m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		if (m_bSelected) pd2dbrText->SetColor(D2D1::ColorF(D2D1::ColorF::White, 1.0f));
		else pd2dbrText->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 1.0f));
		
		pd2dDeviceContext->DrawTextW(m_text.c_str(), static_cast<UINT32>(m_text.length()), m_pTextFormat, m_rTextInput, pd2dbrText);
	}
}

bool CTextInput::CheckChlick(HWND hWnd, POINT CursorPos)
{
	m_bSelected = false;

	// ���콺 Ŀ�� ��ġ ���
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

	// �浹Ȯ��
	if (mousePos.x > m_fLeft && mousePos.x < m_fRight)
	{
		if (mousePos.y > m_fTop && mousePos.y < m_fBottom)
		{
			m_bSelected = true;
		}
	}

	return(m_bSelected);
}
