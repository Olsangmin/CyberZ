#pragma once

class CMachine
{
public:
	CMachine() {}
	~CMachine() {}

private:
	float					m_fWidth = 100.f;
	float					m_fHeight = 100.f;

	float					m_fLeft = 0.f;
	float					m_fRight = m_fLeft + m_fWidth;
	float					m_fTop = 0.f;
	float					m_fBottom = m_fBottom + m_fHeight;

	D2D1_RECT_F				m_rTextInput = { 0.f , 0.f, 0.f, 0.f };

	D2D1_POINT_2F			m_d2d1ImageLoc = { 0, 0 };
	D2D1_RECT_F				m_image = { 0, 0, 130, 110 };

	S2_COM_STATE			m_ComState = TURNOFF;

public:
	void SetPosition(float x, float y);
	void SetSize(float width, float height);

	void SetState(S2_COM_STATE state) { m_ComState = state; }
	S2_COM_STATE GetState() { return(m_ComState); }

	void Draw(ID2D1DeviceContext2* pd2dDeviceContext, ID2D1Effect* pd2dfxBitmapSource, ID2D1SolidColorBrush* pd2dbrBorder);

};

