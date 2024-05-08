
#include "stdafx.h"
#include "GUI.h"

CUI::CUI()
{
}

CUI::~CUI()
{

}

void CUI::Release()
{
	if (m_pd2dbrBackground) m_pd2dbrBackground->Release();
	if (m_pd2dbrBorder) m_pd2dbrBorder->Release();
	if (m_pdwFont) m_pdwFont->Release();
	if (m_pdwTextLayout) m_pdwTextLayout->Release();
	if (m_pd2dbrText) m_pd2dbrText->Release();

	if (m_pd2dDeviceContext) m_pd2dDeviceContext->Release();
	if (m_pd2dDevice) m_pd2dDevice->Release();
	if (m_pdWriteFactory) m_pdWriteFactory->Release();
	if (m_pd3d11On12Device) m_pd3d11On12Device->Release();
	if (m_pd3d11DeviceContext) m_pd3d11DeviceContext->Release();
	if (m_pd2dFactory) m_pd2dFactory->Release();

	for (int i = 0; i < m_nSwapChainBuffers; i++)
	{
		if (m_ppd3d11WrappedBackBuffers[i]) m_ppd3d11WrappedBackBuffers[i]->Release();
		if (m_ppd2dRenderTargets[i]) m_ppd2dRenderTargets[i]->Release();
	}

	//Image
	if (m_pd2dfxBitmapSource) m_pd2dfxBitmapSource->Release();
	if (m_pd2dfxGaussianBlur) m_pd2dfxGaussianBlur->Release();
	if (m_pd2dfxEdgeDetection) m_pd2dfxEdgeDetection->Release();
	if (m_pd2dsbDrawingState) m_pd2dsbDrawingState->Release();
	if (m_pwicFormatConverter) m_pwicFormatConverter->Release();
	if (m_pwicImagingFactory) m_pwicImagingFactory->Release();
}

void CUI::CreateDirect2DDevice(HWND m_hWnd, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12CommandQueue* m_pd3dCommandQueue, ID3D12Resource* m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers])
{
	UINT nD3D11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG) || defined(DBG)
	nD3D11DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	ID3D11Device* pd3d11Device = NULL;
	ID3D12CommandQueue* ppd3dCommandQueues[] = { m_pd3dCommandQueue };
	HRESULT hResult = ::D3D11On12CreateDevice(pd3dDevice, nD3D11DeviceFlags, NULL, 0, reinterpret_cast<IUnknown**>(ppd3dCommandQueues), _countof(ppd3dCommandQueues), 0, &pd3d11Device, &m_pd3d11DeviceContext, NULL);
	hResult = pd3d11Device->QueryInterface(__uuidof(ID3D11On12Device), (void**)&m_pd3d11On12Device);
	if (pd3d11Device) pd3d11Device->Release();

	D2D1_FACTORY_OPTIONS nD2DFactoryOptions = { D2D1_DEBUG_LEVEL_NONE };
#if defined(_DEBUG) || defined(DBG)
	nD2DFactoryOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
	ID3D12InfoQueue* pd3dInfoQueue = NULL;
	if (SUCCEEDED(pd3dDevice->QueryInterface(IID_PPV_ARGS(&pd3dInfoQueue))))
	{
		D3D12_MESSAGE_SEVERITY pd3dSeverities[] =
		{
			D3D12_MESSAGE_SEVERITY_INFO,
		};

		D3D12_MESSAGE_ID pd3dDenyIds[] =
		{
			D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE,
		};

		D3D12_INFO_QUEUE_FILTER d3dInforQueueFilter = { };
		d3dInforQueueFilter.DenyList.NumSeverities = _countof(pd3dSeverities);
		d3dInforQueueFilter.DenyList.pSeverityList = pd3dSeverities;
		d3dInforQueueFilter.DenyList.NumIDs = _countof(pd3dDenyIds);
		d3dInforQueueFilter.DenyList.pIDList = pd3dDenyIds;

		pd3dInfoQueue->PushStorageFilter(&d3dInforQueueFilter);
	}
	pd3dInfoQueue->Release();
#endif

	hResult = ::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &nD2DFactoryOptions, (void**)&m_pd2dFactory);

	IDXGIDevice* pdxgiDevice = NULL;
	hResult = m_pd3d11On12Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&pdxgiDevice);
	hResult = m_pd2dFactory->CreateDevice(pdxgiDevice, &m_pd2dDevice);
	hResult = m_pd2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_pd2dDeviceContext);
	hResult = ::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&m_pdWriteFactory);
	if (pdxgiDevice) pdxgiDevice->Release();

	m_pd2dDeviceContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

	m_pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(0.3f, 0.0f, 0.0f, 0.5f), &m_pd2dbrBackground);
	m_pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White, 1.0f), &m_pd2dbrBorder);

	hResult = m_pdWriteFactory->CreateTextFormat(L"ComicSans", NULL, DWRITE_FONT_WEIGHT_DEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 35.0f, L"en-US", &m_pdwFont);
	hResult = m_pdwFont->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	hResult = m_pdwFont->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	m_pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White, 1.0f), &m_pd2dbrText);
	hResult = m_pdWriteFactory->CreateTextLayout(L"텍스트 레이아웃", 8, m_pdwFont, 4096.0f, 4096.0f, &m_pdwTextLayout);

	float fDpi = (float)GetDpiForWindow(m_hWnd);
	D2D1_BITMAP_PROPERTIES1 d2dBitmapProperties = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), fDpi, fDpi);

	for (UINT i = 0; i < m_nSwapChainBuffers; i++)
	{
		D3D11_RESOURCE_FLAGS d3d11Flags = { D3D11_BIND_RENDER_TARGET };
		m_pd3d11On12Device->CreateWrappedResource(m_ppd3dSwapChainBackBuffers[i], &d3d11Flags, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT, IID_PPV_ARGS(&m_ppd3d11WrappedBackBuffers[i]));
		IDXGISurface* pdxgiSurface = NULL;
		m_ppd3d11WrappedBackBuffers[i]->QueryInterface(__uuidof(IDXGISurface), (void**)&pdxgiSurface);
		m_pd2dDeviceContext->CreateBitmapFromDxgiSurface(pdxgiSurface, &d2dBitmapProperties, &m_ppd2dRenderTargets[i]);
		if (pdxgiSurface) pdxgiSurface->Release();
	}



	// Image Render
	CoInitialize(NULL);
	hResult = ::CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory), (void**)&m_pwicImagingFactory);

	hResult = m_pd2dFactory->CreateDrawingStateBlock(&m_pd2dsbDrawingState);
	hResult = m_pd2dDeviceContext->CreateEffect(CLSID_D2D1BitmapSource, &m_pd2dfxBitmapSource);
	hResult = m_pd2dDeviceContext->CreateEffect(CLSID_D2D1GaussianBlur, &m_pd2dfxGaussianBlur);
	hResult = m_pd2dDeviceContext->CreateEffect(CLSID_D2D1EdgeDetection, &m_pd2dfxEdgeDetection);

	IWICBitmapDecoder* pwicBitmapDecoder;
	hResult = m_pwicImagingFactory->CreateDecoderFromFilename(L"Image/CardKey.png", NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pwicBitmapDecoder);
	IWICBitmapFrameDecode* pwicFrameDecode;
	pwicBitmapDecoder->GetFrame(0, &pwicFrameDecode);
	m_pwicImagingFactory->CreateFormatConverter(&m_pwicFormatConverter);
	m_pwicFormatConverter->Initialize(pwicFrameDecode, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);
	m_pd2dfxBitmapSource->SetValue(D2D1_BITMAPSOURCE_PROP_WIC_BITMAP_SOURCE, m_pwicFormatConverter);

	m_pd2dfxGaussianBlur->SetInputEffect(0, m_pd2dfxBitmapSource);

	m_pd2dfxEdgeDetection->SetInputEffect(0, m_pd2dfxBitmapSource);
	m_pd2dfxEdgeDetection->SetValue(D2D1_EDGEDETECTION_PROP_STRENGTH, 0.5f);
	m_pd2dfxEdgeDetection->SetValue(D2D1_EDGEDETECTION_PROP_BLUR_RADIUS, 0.0f);
	m_pd2dfxEdgeDetection->SetValue(D2D1_EDGEDETECTION_PROP_MODE, D2D1_EDGEDETECTION_MODE_SOBEL);
	m_pd2dfxEdgeDetection->SetValue(D2D1_EDGEDETECTION_PROP_OVERLAY_EDGES, false);
	m_pd2dfxEdgeDetection->SetValue(D2D1_EDGEDETECTION_PROP_ALPHA_MODE, D2D1_ALPHA_MODE_PREMULTIPLIED);

	m_pd2dfxGaussianBlur->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, 0.0f);
	m_nDrawEffectImage = 0;

	if (pwicBitmapDecoder) pwicBitmapDecoder->Release();
	if (pwicFrameDecode) pwicFrameDecode->Release();
}

void CUI::DrawUI(UINT m_nSwapChainBufferIndex)
{
	//Direct2D Drawing

	m_pd2dDeviceContext->SetTarget(m_ppd2dRenderTargets[m_nSwapChainBufferIndex]);
	ID3D11Resource* ppd3dResources[] = { m_ppd3d11WrappedBackBuffers[m_nSwapChainBufferIndex] };
	m_pd3d11On12Device->AcquireWrappedResources(ppd3dResources, _countof(ppd3dResources));

	m_pd2dDeviceContext->BeginDraw();

	m_pd2dDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());

	//-----------------------------

	D2D1_SIZE_F szRenderTarget = m_ppd2dRenderTargets[m_nSwapChainBufferIndex]->GetSize();

	D2D1_RECT_F rcUpperText = D2D1::RectF(0, 0, szRenderTarget.width, szRenderTarget.height * 0.25f);
	m_pd2dDeviceContext->DrawTextW(L"testUI", (UINT32)wcslen(L"testUI"), m_pdwFont, &rcUpperText, m_pd2dbrText);

	m_pd2dDeviceContext->EndDraw();

	//-----------------------------

	m_pd3d11On12Device->ReleaseWrappedResources(ppd3dResources, _countof(ppd3dResources));
	m_pd3d11DeviceContext->Flush();

}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void CFirstSceneUI::DrawUI(UINT m_nSwapChainBufferIndex)
{
	m_pd2dDeviceContext->SetTarget(m_ppd2dRenderTargets[m_nSwapChainBufferIndex]);
	ID3D11Resource* ppd3dResources[] = { m_ppd3d11WrappedBackBuffers[m_nSwapChainBufferIndex] };
	m_pd3d11On12Device->AcquireWrappedResources(ppd3dResources, _countof(ppd3dResources));

	m_pd2dDeviceContext->BeginDraw();
	m_pd2dDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());

	//Direct2D Drawing
	UISet(m_nSwapChainBufferIndex);

	
	m_pd2dDeviceContext->EndDraw();
	m_pd3d11On12Device->ReleaseWrappedResources(ppd3dResources, _countof(ppd3dResources));
	m_pd3d11DeviceContext->Flush();
}

void CFirstSceneUI::UISet(UINT m_nSwapChainBufferIndex)
{
	
	//D2D1_SIZE_F szRenderTarget = m_ppd2dRenderTargets[m_nSwapChainBufferIndex]->GetSize();
	//WCHAR InfoText[] = L"Press 'R' To Ready";
	//D2D1_RECT_F rcUpperText = D2D1::RectF(0, 0, szRenderTarget.width, szRenderTarget.height * 0.25f);
	//m_pd2dDeviceContext->DrawTextW(InfoText, (UINT32)wcslen(InfoText), m_pdwFont, &rcUpperText, m_pd2dbrText);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CPlaySceneUI::DrawUI(UINT m_nSwapChainBufferIndex)
{
	m_pd2dDeviceContext->SetTarget(m_ppd2dRenderTargets[m_nSwapChainBufferIndex]);
	ID3D11Resource* ppd3dResources[] = { m_ppd3d11WrappedBackBuffers[m_nSwapChainBufferIndex] };
	m_pd3d11On12Device->AcquireWrappedResources(ppd3dResources, _countof(ppd3dResources));

	m_pd2dDeviceContext->BeginDraw();
	m_pd2dDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());

	//Direct2D Drawing
	UISet(m_nSwapChainBufferIndex);

	m_pd2dDeviceContext->EndDraw();
	m_pd3d11On12Device->ReleaseWrappedResources(ppd3dResources, _countof(ppd3dResources));
	m_pd3d11DeviceContext->Flush();

}

void CPlaySceneUI::UISet(UINT m_nSwapChainBufferIndex)
{
	D2D1_SIZE_F szRenderTarget = m_ppd2dRenderTargets[m_nSwapChainBufferIndex]->GetSize();

	// 점령미션 바 상단 문구
	D2D1_RECT_F rcUpperText = D2D1::RectF(40, 20, 200, 40);
	WCHAR MissionText[] = L"Progress";
	m_pd2dDeviceContext->DrawTextW(MissionText, (UINT32)wcslen(MissionText), m_pdwFont, &rcUpperText, m_pd2dbrText);

	for (int i = 0; i < 3; i++)	MissionProgressBar(i);
	if (m_bStaminaBarOn) StaminaBarUI();
	KeyCardUI();
	
}

void CPlaySceneUI::MissionProgressBar(int MissionNum)
{
	float gab = 35.5;

	float width = 30.5;
	float height = 30.f;

	//게이지 바
	D2D1_RECT_F* rcMissionBar;
	rcMissionBar = new D2D1_RECT_F;
	rcMissionBar->top = 60.5f + (MissionNum * gab);
	rcMissionBar->left = 30.f;
	rcMissionBar->right = 30.f + m_fMissionGauge[MissionNum];
	rcMissionBar->bottom = 89.5f + (MissionNum * gab);

	m_pd2dbrBorder->SetColor(D2D1::ColorF(0x00ff00, 0.6f));
	m_pd2dDeviceContext->FillRectangle(rcMissionBar, m_pd2dbrBorder);

	// 게이지 바 프레임
	D2D1_RECT_F* rcMissionBarFrame;
	rcMissionBarFrame = new D2D1_RECT_F;
	rcMissionBarFrame->top = 60.f + (MissionNum * gab);
	rcMissionBarFrame->left = 30.0f;
	rcMissionBarFrame->right = 400.f;
	rcMissionBarFrame->bottom = 90.0f + (MissionNum * gab);

	m_pd2dbrBorder->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 1.0f));
	m_pd2dDeviceContext->DrawRectangle(rcMissionBarFrame, m_pd2dbrBorder);

	delete rcMissionBar;
	delete rcMissionBarFrame;

}

void CPlaySceneUI::KeyCardUI()
{
	D2D1_RECT_F* rcMissionBarFrame;
	rcMissionBarFrame = new D2D1_RECT_F;
	rcMissionBarFrame->top = FRAME_BUFFER_HEIGHT - 200.f;
	rcMissionBarFrame->left = FRAME_BUFFER_WIDTH - 270.0f;
	rcMissionBarFrame->right = FRAME_BUFFER_WIDTH - 50.f;
	rcMissionBarFrame->bottom = FRAME_BUFFER_HEIGHT - 50.0f;

	m_pd2dbrBorder->SetColor(D2D1::ColorF(D2D1::ColorF::White, 1.0f));
	m_pd2dDeviceContext->DrawRectangle(rcMissionBarFrame, m_pd2dbrBorder);

	delete rcMissionBarFrame;

	float top = 1650.f;
	float left = 850.f;
	float gab = 100.f;

	D2D1_RECT_F rcUpperText = D2D1::RectF(top, left, top+gab,left+gab);
	WCHAR MissionText[] = L"ITEM";
	m_pd2dDeviceContext->DrawTextW(MissionText, (UINT32)wcslen(MissionText), m_pdwFont, &rcUpperText, m_pd2dbrText);

	if (m_bcard)
	{
		// KeyCard Image
		D2D_POINT_2F d2dPoint = { FRAME_BUFFER_WIDTH - 250.f, FRAME_BUFFER_HEIGHT - 180.f };
		D2D_RECT_F d2dRect = { 0.0f, 0.0f, 200.0f, 130.0f };
		m_pd2dDeviceContext->DrawImage((m_nDrawEffectImage == 0) ? m_pd2dfxGaussianBlur : m_pd2dfxEdgeDetection, &d2dPoint, &d2dRect);
	}


}

void CPlaySceneUI::StaminaBarUI()
{
	//게이지 바
	D2D1_RECT_F* rcStaminaBar;
	rcStaminaBar = new D2D1_RECT_F;
	rcStaminaBar->top = 160.5f;
	rcStaminaBar->left = 130.f;
	rcStaminaBar->right = 130.f + m_fStaminaRange;
	rcStaminaBar->bottom = 189.5f;

	m_pd2dbrBorder->SetColor(D2D1::ColorF(D2D1::ColorF::White, 0.6f));
	m_pd2dDeviceContext->FillRectangle(rcStaminaBar, m_pd2dbrBorder);

	// 게이지 바 프레임
	D2D1_RECT_F* rcStaminaBarFrame;
	rcStaminaBarFrame = new D2D1_RECT_F;
	rcStaminaBarFrame->top = 160.f;
	rcStaminaBarFrame->left = 130.0f;
	rcStaminaBarFrame->right = 800.f;
	rcStaminaBarFrame->bottom = 190.0f;

	m_pd2dbrBorder->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 1.0f));
	m_pd2dDeviceContext->DrawRectangle(rcStaminaBarFrame, m_pd2dbrBorder);

	delete rcStaminaBar;
	delete rcStaminaBarFrame;

}

