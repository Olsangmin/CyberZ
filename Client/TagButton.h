#pragma once
class CTagButton
{
public:
	CTagButton() {}
	CTagButton(float x, float y, float width, float height);
	CTagButton(D2D1_RECT_F rect);
	~CTagButton() {};

private:
	float					m_fWidth = 100.f;
	float					m_fHeight = 100.f;

	float					m_fLeft = 0.f;
	float					m_fTop = 0.f;
	float					m_fRight = m_fLeft + m_fWidth;
	float					m_fBottom = m_fBottom + m_fHeight;


	D2D1_RECT_F				m_rButtonRect = { 0.f , 0.f, 0.f, 0.f };
	D2D1_COLOR_F			customColor = D2D1::ColorF(0.0f, 0.5f, 0.5f, 1.0f);
public:
	void SetPosition(float x, float y);
	void SetSize(float width, float height);

	D2D1_RECT_F GetRect() { return(m_rButtonRect); }

	void Draw(ID2D1DeviceContext2* pd2dDeviceContext, ID2D1SolidColorBrush* pd2dbrBorder, int pUISphare);

	bool				m_bMouseOn = false;
	bool CheckMouseOn(HWND hWnd, POINT CursorPos);
};

