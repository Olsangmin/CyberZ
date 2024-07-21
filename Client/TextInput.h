#pragma once
class CTextInput
{
public:
	CTextInput() {}
	CTextInput(float x, float y, float width, float height, const wchar_t* text);


	~CTextInput() {}
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

	std::wstring			m_text;

	IDWriteTextFormat* m_pTextFormat = NULL;

public:
	bool				m_bSelected = false;

public:
	void SetPosition(float x, float y) { m_fLeft = x; m_fTop = y; }
	void SetSize(float width, float height); 

	void SetText(const wchar_t* text) { m_text = text; }
	wstring GetText() { return (m_text); }

	void CreateTextFormat(IDWriteFactory* pDWriteFactory, float fontSize);
	void Draw(ID2D1DeviceContext2* pd2dDeviceContext, ID2D1SolidColorBrush* pd2dbrText, ID2D1SolidColorBrush* pd2dbrBorde);

	bool CheckChlick(HWND hWnd, POINT CursorPos);
};

