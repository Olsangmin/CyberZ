
#include "stdafx.h"
#include "ProgressBar.h"


CProgressBar::~CProgressBar()
{
	if (m_pTextFormat) m_pTextFormat->Release();
}

void CProgressBar::SetPosition(float x, float y)
{
	m_fLeft = x;
	m_fTop = y;
}

void CProgressBar::SetSize(float width, float height)
{
	m_fWidth = width;
	m_fHeight = height;
	m_fRight = m_fLeft + width;
	m_fBottom = m_fTop + height;
}

void CProgressBar::SetText(const wchar_t* text)
{
	m_text = text;
}

void CProgressBar::SetColor(D2D1_COLOR_F fClolor, D2D1_COLOR_F pColor)
{
	FrameColor = fClolor;
	ProcessColor = pColor;
}


void CProgressBar::CreateTextFormat(IDWriteFactory* pDWriteFactory, float fontSize)
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

void CProgressBar::Draw(ID2D1DeviceContext2* pd2dDeviceContext, ID2D1SolidColorBrush* pd2dbrText, ID2D1SolidColorBrush* pd2dbrBorder)
{

	//text
	m_rTextInput = { m_fLeft + 5, m_fTop, m_fRight, m_fBottom };
	if (m_pTextFormat)
	{
		m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pd2dbrText->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 1.0f));

		pd2dDeviceContext->DrawTextW(m_text.c_str(), static_cast<UINT32>(m_text.length()), m_pTextFormat, m_rTextInput, pd2dbrText);
	}

	// Bar
	m_rFrameRect = { m_fLeft, m_fTop, m_fRight, m_fBottom };
	m_rProcessRect = { m_fLeft, m_fTop, m_fLeft + m_fMissionProcess, m_fBottom };


	pd2dbrBorder->SetColor(ProcessColor);
	pd2dDeviceContext->FillRectangle(m_rProcessRect, pd2dbrBorder);

	pd2dbrBorder->SetColor(FrameColor);
	pd2dDeviceContext->DrawRectangle(m_rFrameRect, pd2dbrBorder, 4.f);
	
	if (m_fMissionProcess >= m_fWidth) m_done = true;
	else m_done = false;
}

