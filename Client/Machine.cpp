
#include "stdafx.h"
#include "Machine.h"

void CMachine::SetPosition(float x, float y)
{
	m_fLeft = x;
	m_fTop = y;
}

void CMachine::SetSize(float width, float height)
{
	m_fRight = m_fLeft + width;
	m_fBottom = m_fTop + height;
}

void CMachine::Draw(ID2D1DeviceContext2* pd2dDeviceContext, ID2D1Effect* pd2dfxBitmapSource, ID2D1SolidColorBrush* pd2dbrBorder)
{
	m_d2d1ImageLoc = { m_fLeft , m_fTop };


	//BG
	pd2dDeviceContext->DrawImage(pd2dfxBitmapSource, &m_d2d1ImageLoc, &m_image);

}
