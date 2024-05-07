#pragma once

class CUI
{
public:
	CUI();
	~CUI();

	void Release();

public:
	static const UINT			m_nSwapChainBuffers = 2;

	ID3D11On12Device* m_pd3d11On12Device = NULL;
	ID3D11DeviceContext* m_pd3d11DeviceContext = NULL;
	ID2D1Factory3* m_pd2dFactory = NULL;
	IDWriteFactory* m_pdWriteFactory = NULL;
	ID2D1Device2* m_pd2dDevice = NULL;
	ID2D1DeviceContext2* m_pd2dDeviceContext = NULL;

	ID3D11Resource* m_ppd3d11WrappedBackBuffers[m_nSwapChainBuffers];
	ID2D1Bitmap1* m_ppd2dRenderTargets[m_nSwapChainBuffers];

	ID2D1SolidColorBrush* m_pd2dbrBackground = NULL;
	ID2D1SolidColorBrush* m_pd2dbrBorder = NULL;
	IDWriteTextFormat* m_pdwFont = NULL;
	IDWriteTextLayout* m_pdwTextLayout = NULL;
	ID2D1SolidColorBrush* m_pd2dbrText = NULL;


	//Image
	IWICImagingFactory* m_pwicImagingFactory = NULL;
	ID2D1Effect* m_pd2dfxBitmapSource = NULL;
	ID2D1Effect* m_pd2dfxGaussianBlur = NULL;
	ID2D1Effect* m_pd2dfxEdgeDetection = NULL;
	ID2D1DrawingStateBlock1* m_pd2dsbDrawingState = NULL;
	IWICFormatConverter* m_pwicFormatConverter = NULL;
	int							m_nDrawEffectImage = 0;

public:
	int m_nMissions = 3;
	float m_fMissionGauge[3] = { 0 };

	bool m_bcard = false;

public:
	void CreateDirect2DDevice(HWND m_hWnd, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12CommandQueue* m_pd3dCommandQueue, ID3D12Resource* m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers]);
	virtual void DrawUI(UINT m_nSwapChainBufferIndex);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CFirstSceneUI : public CUI
{
public:
	CFirstSceneUI() {}
	~CFirstSceneUI() {}

public:
	void DrawUI(UINT m_nSwapChainBufferIndex);
	void UISet(UINT m_nSwapChainBufferIndex);

};

class CPlaySceneUI : public CUI
{
public:
	CPlaySceneUI() {}
	~CPlaySceneUI() {}

public:
	void MissionProgressBar(int MissionNum);
	void KeyCardUI();


	void DrawUI(UINT m_nSwapChainBufferIndex);
	void UISet(UINT m_nSwapChainBufferIndex);

};