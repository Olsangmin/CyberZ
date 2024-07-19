
#include "stdafx.h"
#include "Button.h"

CButton::CButton(float x, float y, float width, float height, const wchar_t* text)
{
	m_fLeft = x;
	m_fRight = x + width;
	m_fTop = y;
	m_fBottom = y + height;

	m_rButtonBox = { m_fLeft , m_fTop, m_fRight, m_fBottom };
	
	m_text = text;
}

CButton::~CButton()
{
}

void CButton::Release()
{
	if (m_pTextFormat) m_pTextFormat->Release();
}


void CButton::SetPosition(float x, float y)
{
	m_fLeft = x;
	m_fTop = y;
}

void CButton::SetSize(float width, float height)
{
	m_fRight = m_fLeft + width;
	m_fBottom = m_fTop + height;
}

void CButton::SetText(const wchar_t* text)
{
	m_text = text;
}

void CButton::CreateTextFormat(IDWriteFactory* pDWriteFactory, float fontSize)
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

void CButton::Draw(ID2D1DeviceContext2* pd2dDeviceContext, ID2D1Effect* pd2dfxBitmapSource, ID2D1SolidColorBrush* pd2dbrText)
{
	m_rButtonBox = { m_fLeft, m_fTop, m_fRight, m_fBottom };

	//Image
	D2D_POINT_2F d2dPoint = { m_fLeft, m_fTop };
	pd2dDeviceContext->DrawImage(pd2dfxBitmapSource, &d2dPoint, &m_rButtonBox);
	

	//text
	if (m_pTextFormat)
	{
		m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pd2dDeviceContext->DrawTextW(m_text.c_str(), static_cast<UINT32>(m_text.length()), m_pTextFormat, m_rButtonBox, pd2dbrText);
	}
}
