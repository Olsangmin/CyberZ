#pragma once
class CProgressBar
{
public:
	CProgressBar() {}
	~CProgressBar();
	
private:
	float					m_fWidth = 100.f;
	float					m_fHeight = 100.f;

	float					m_fLeft = 0.f;
	float					m_fTop = 0.f;
	float					m_fRight = m_fLeft + m_fWidth;
	float					m_fBottom = m_fBottom + m_fHeight;

	D2D1_RECT_F				m_rTextInput = { 0.f , 0.f, 0.f, 0.f };
	D2D1_RECT_F				m_rFrameRect = { 0.f , 0.f, 0.f, 0.f };
	D2D1_RECT_F				m_rProcessRect = { 0.f , 0.f, 0.f, 0.f };

	bool					m_done = false;
	std::wstring			m_text;
	IDWriteTextFormat*		m_pTextFormat = NULL;
	
	D2D1_COLOR_F			FrameColor = D2D1::ColorF(0.0f, 0.5f, 0.5f, 1.0f);
	D2D1_COLOR_F			ProcessColor = D2D1::ColorF(0.0f, 0.5f, 0.5f, 1.0f);

	float					m_fMissionProcess = 0;

public:
	void SetPosition(float x, float y);
	void SetSize(float width, float height);
	void SetText(const wchar_t* text);
	void SetColor(D2D1_COLOR_F fClolor, D2D1_COLOR_F pColor);
	void SetProcess(float range) { m_fMissionProcess = range; };

	void SetDone(bool Done) { m_done = Done; }
	bool GetDone() { return(m_done); }

	void CreateTextFormat(IDWriteFactory* pDWriteFactory, float fontSize);
	void Draw(ID2D1DeviceContext2* pd2dDeviceContext, ID2D1SolidColorBrush* pd2dbrText, ID2D1SolidColorBrush* pd2dbrBorder);

};

