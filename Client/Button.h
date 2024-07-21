#pragma once

class CButton
{
public:
	CButton() {}
	CButton(float x, float y, float width, float height, const wchar_t* text);
	
	~CButton();
	void Release();

private:

	float					m_fWidth = 100.f;
	float					m_fHeight = 100.f;
							  
	float					m_fLeft = 0.f;
	float					m_fRight = m_fLeft + m_fWidth;
	float					m_fTop = 0.f;
	float					m_fBottom = m_fBottom + m_fHeight;

	D2D1_RECT_F				m_rTextInput = { 0.f , 0.f, 0.f, 0.f };
	D2D1_RECT_F				m_rButtonBox = { 0.f , 0.f, 0.f, 0.f };
	D2D1_ROUNDED_RECT		m_rRoundButtonBox = { m_rButtonBox , 0.f, 0.f };
	
	std::wstring			m_imagePath;
	
	std::wstring			m_text;
	IDWriteTextFormat*		m_pTextFormat = NULL;

public:
	void SetPosition(float x, float y);
	void SetSize(float width, float height);
	void SetText(const wchar_t* text);

	void CreateTextFormat(IDWriteFactory* pDWriteFactory, float fontSize);
	void Draw(ID2D1DeviceContext2* pd2dDeviceContext, ID2D1SolidColorBrush* pd2dbrText, ID2D1SolidColorBrush* pd2dbrBorder);

	bool				m_bSelected = false;
	bool CheckChlick(HWND hWnd, POINT CursorPos);

};

