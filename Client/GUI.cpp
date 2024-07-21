
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
	if (m_ppButton)
	{
		for (int i = 0; i < m_nButton; i++) if (m_ppButton[i]) m_ppButton[i]->Release();
		delete[] m_ppButton;
	}
	if (m_ppTextInputBox)
	{
		for (int i = 0; i < m_nTextInputBox; i++) if (m_ppTextInputBox[i]) m_ppTextInputBox[i]->Release();
		delete[] m_ppTextInputBox;
	}

	//Image
	if (m_pd2dfxBitmapSource) m_pd2dfxBitmapSource->Release();
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

	hResult = m_pdWriteFactory->CreateTextFormat(L"ComicSans", NULL, DWRITE_FONT_WEIGHT_DEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 30.0f, L"en-US", &m_pdwFont);
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
	
	IWICBitmapDecoder* pwicBitmapDecoder;
	hResult = m_pwicImagingFactory->CreateDecoderFromFilename(L"Image/CardKey.png", NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pwicBitmapDecoder);
	IWICBitmapFrameDecode* pwicFrameDecode;
	pwicBitmapDecoder->GetFrame(0, &pwicFrameDecode);
	m_pwicImagingFactory->CreateFormatConverter(&m_pwicFormatConverter);
	m_pwicFormatConverter->Initialize(pwicFrameDecode, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);
	m_pd2dfxBitmapSource->SetValue(D2D1_BITMAPSOURCE_PROP_WIC_BITMAP_SOURCE, m_pwicFormatConverter);

	m_nDrawEffectImage = 0;

	if (pwicBitmapDecoder) pwicBitmapDecoder->Release();
	if (pwicFrameDecode) pwicFrameDecode->Release();
}

void CUI::LoadUIImage(const wchar_t* filename, IWICImagingFactory* pwicImagingFactory, ID2D1Effect* pd2dfxBitmapSource)
{
	IWICBitmapDecoder* pwicBitmapDecoder;
	HRESULT hResult = pwicImagingFactory->CreateDecoderFromFilename(filename, NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pwicBitmapDecoder);
	if (FAILED(hResult)) return;  // 오류 처리 추가

	IWICBitmapFrameDecode* pwicFrameDecode;
	hResult = pwicBitmapDecoder->GetFrame(0, &pwicFrameDecode);
	if (FAILED(hResult)) return;  // 오류 처리 추가

	IWICFormatConverter* pwicFormatConverter;
	hResult = pwicImagingFactory->CreateFormatConverter(&pwicFormatConverter);
	if (FAILED(hResult)) return;  // 오류 처리 추가

	hResult = pwicFormatConverter->Initialize(pwicFrameDecode, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);
	if (FAILED(hResult)) return;  // 오류 처리 추가

	// pd2dfxBitmapSource의 입력 비트맵을 새로운 이미지로 업데이트
	pd2dfxBitmapSource->SetValue(D2D1_BITMAPSOURCE_PROP_WIC_BITMAP_SOURCE, pwicFormatConverter);

	// 해제
	pwicFormatConverter->Release();
	pwicFrameDecode->Release();
	pwicBitmapDecoder->Release();
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
	D2D1_SIZE_F szRenderTarget = m_ppd2dRenderTargets[m_nSwapChainBufferIndex]->GetSize();

	CheckEnter();
}

void CFirstSceneUI::CheckEnter()
{
	
#ifdef SMALL_WINDOW_SCREEN
	//Small window
	m_pdWriteFactory->CreateTextFormat(L"ComicSans", NULL, DWRITE_FONT_WEIGHT_DEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 20.0f, L"en-US", &m_pdwFont);
	
	top = FRAME_BUFFER_HEIGHT / 3 + 95;
	left = FRAME_BUFFER_WIDTH / 3 - 90;
	width = 80;
	height = 10;
	gab = 165;

#else
	m_pdWriteFactory->CreateTextFormat(L"ComicSans", NULL, DWRITE_FONT_WEIGHT_DEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 40.0f, L"en-US", &m_pdwFont);

	top = FRAME_BUFFER_HEIGHT / 3 + 200;
	left = FRAME_BUFFER_WIDTH / 3 - 90;
	width = 180;
	height = 10;
	gab = 325;

#endif // SMALL_WINDOW_SCREEN

	
	if (m_bPlayerOn[0])
	{
		WCHAR InfoText[] = L"Player 1";
		D2D1_RECT_F rcPlayer0 = D2D1::RectF(left, top, left + width, top + height);
		m_pd2dDeviceContext->DrawTextW(InfoText, (UINT32)wcslen(InfoText), m_pdwFont, &rcPlayer0, m_pd2dbrText);
	}

	if (m_bPlayerOn[1])
	{
		WCHAR InfoText1[] = L"Player 2";
		D2D1_RECT_F rcPlayer1 = D2D1::RectF(left + gab, top, left + width + gab, top + height);
		m_pd2dDeviceContext->DrawTextW(InfoText1, (UINT32)wcslen(InfoText1), m_pdwFont, &rcPlayer1, m_pd2dbrText);
	}

	if (m_bPlayerOn[2])
	{
		WCHAR InfoText2[] = L"Player 3";
		D2D1_RECT_F rcPlayer2 = D2D1::RectF(left + gab * 2, top, left + width + gab * 2, top + height);
		m_pd2dDeviceContext->DrawTextW(InfoText2, (UINT32)wcslen(InfoText2), m_pdwFont, &rcPlayer2, m_pd2dbrText);
	}

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//========================================================================================//
// First Round

void CFirstRoundSceneUI::DrawUI(UINT m_nSwapChainBufferIndex)
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

void CFirstRoundSceneUI::UISet(UINT m_nSwapChainBufferIndex)
{
	D2D1_SIZE_F szRenderTarget = m_ppd2dRenderTargets[m_nSwapChainBufferIndex]->GetSize();

	MissionText();
	for (int i = 0; i < 3; i++)	MissionProgressBar(i);
	if (m_bStaminaBarOn) StaminaBarUI();
	kyecardUI();
	ItemUI();
	
}

void CFirstRoundSceneUI::MissionText()
{ 
	float gab = 35.5;

	D2D1_RECT_F rcUpperText = D2D1::RectF(100.f, 30.f, 300.f, 60.f);
	WCHAR MissionText[] = L"Area 1";
	m_pd2dbrText->SetColor(D2D1::ColorF(D2D1::ColorF::White, 1.0f));
	m_pd2dDeviceContext->DrawTextW(MissionText, (UINT32)wcslen(MissionText), m_pdwFont, &rcUpperText, m_pd2dbrText);

	D2D1_RECT_F rcUpperText2 = D2D1::RectF(100.f, 30.f + (gab * 1), 300.f, 60.f + (gab * 1));
	WCHAR MissionText1[] = L"Area 2";
	m_pd2dbrText->SetColor(D2D1::ColorF(D2D1::ColorF::White, 1.0f));
	m_pd2dDeviceContext->DrawTextW(MissionText1, (UINT32)wcslen(MissionText1), m_pdwFont, &rcUpperText2, m_pd2dbrText);

	D2D1_RECT_F rcUpperText3 = D2D1::RectF(100.f, 30.f + (gab * 2), 300.f, 60.f + (gab * 2));
	WCHAR MissionText2[] = L"Area 3";
	m_pd2dbrText->SetColor(D2D1::ColorF(D2D1::ColorF::White, 1.0f));
	m_pd2dDeviceContext->DrawTextW(MissionText2, (UINT32)wcslen(MissionText2), m_pdwFont, &rcUpperText3, m_pd2dbrText);

}

void CFirstRoundSceneUI::MissionProgressBar(int MissionNum)
{
	float gab = 35.5;

	float width = 30.5;
	float height = 30.f;

	//게이지 바
	D2D1_RECT_F* rcMissionBar;
	rcMissionBar = new D2D1_RECT_F;
	rcMissionBar->top = 30.5f + (MissionNum * gab);
	rcMissionBar->left = 30.f;
	rcMissionBar->right = 30.f + m_fMissionGauge[MissionNum];
	rcMissionBar->bottom = 59.5f + (MissionNum * gab);

	m_pd2dbrBorder->SetColor(D2D1::ColorF(0x00ff00, 0.6f));
	m_pd2dDeviceContext->FillRectangle(rcMissionBar, m_pd2dbrBorder);

	// 게이지 바 프레임
	D2D1_RECT_F* rcMissionBarFrame;
	rcMissionBarFrame = new D2D1_RECT_F;
	rcMissionBarFrame->top = 30.f + (MissionNum * gab);
	rcMissionBarFrame->left = 30.0f;
	rcMissionBarFrame->right = 400.f;
	rcMissionBarFrame->bottom = 60.0f + (MissionNum * gab);

	m_pd2dbrBorder->SetColor(D2D1::ColorF(D2D1::ColorF::AliceBlue, 1.0f));
	m_pd2dDeviceContext->DrawRectangle(rcMissionBarFrame, m_pd2dbrBorder);

	delete rcMissionBar;
	delete rcMissionBarFrame;

}

void CFirstRoundSceneUI::kyecardUI()
{

	D2D1_RECT_F rcKyeCardRect = { 0.0f, 0.0f, 200.0f, 130.0f };
	D2D_POINT_2F lKeyCard = { 15.f, 130.f };

	if (m_bcard) LoadUIImage(L"Image/CardKey.png", m_pwicImagingFactory, m_pd2dfxBitmapSource);
	else LoadUIImage(L"Image/Card_off.png", m_pwicImagingFactory, m_pd2dfxBitmapSource);
	
	m_pd2dDeviceContext->DrawImage(m_pd2dfxBitmapSource, &lKeyCard, &rcKyeCardRect);

}

void CFirstRoundSceneUI::ItemUI()
{
	D2D1_RECT_F* rcMissionBarFrame;
	rcMissionBarFrame = new D2D1_RECT_F;
	rcMissionBarFrame->top = FRAME_BUFFER_HEIGHT - 200.f;
	rcMissionBarFrame->left = FRAME_BUFFER_WIDTH - 270.0f;
	rcMissionBarFrame->right = FRAME_BUFFER_WIDTH - 50.f;
	rcMissionBarFrame->bottom = FRAME_BUFFER_HEIGHT - 50.0f;

	m_pd2dbrBorder->SetColor(D2D1::ColorF(D2D1::ColorF::GreenYellow, 1.0f));
	m_pd2dDeviceContext->DrawRectangle(rcMissionBarFrame, m_pd2dbrBorder);

	delete rcMissionBarFrame;

	float top = 1650.f;
	float left = 850.f;
	float gab = 100.f;

	D2D1_RECT_F rcUpperText = D2D1::RectF(top, left, top+gab,left+gab);
	WCHAR MissionText[] = L"ITEM";
	m_pd2dbrText->SetColor(D2D1::ColorF(D2D1::ColorF::GreenYellow, 1.0f));
	m_pd2dDeviceContext->DrawTextW(MissionText, (UINT32)wcslen(MissionText), m_pdwFont, &rcUpperText, m_pd2dbrText);


}

void CFirstRoundSceneUI::StaminaBarUI()
{
	float halfsize = m_fMaxStamina / 2;

	//게이지 바
	D2D1_RECT_F* rcStaminaBar;
	rcStaminaBar = new D2D1_RECT_F;
	rcStaminaBar->top = FRAME_BUFFER_HEIGHT - 230.f;
	rcStaminaBar->left = FRAME_BUFFER_WIDTH - 270.0f;
	rcStaminaBar->right = FRAME_BUFFER_WIDTH - 270.0f + m_fStaminaRange;
	rcStaminaBar->bottom = FRAME_BUFFER_HEIGHT - 210.0f;

	m_pd2dbrBorder->SetColor(D2D1::ColorF(D2D1::ColorF::RoyalBlue, 1.0f));
	m_pd2dDeviceContext->FillRectangle(rcStaminaBar, m_pd2dbrBorder);

	// 게이지 바 프레임
	D2D1_RECT_F* rcStaminaBarFrame;
	rcStaminaBarFrame = new D2D1_RECT_F;
	rcStaminaBarFrame->top = FRAME_BUFFER_HEIGHT - 230.f;
	rcStaminaBarFrame->left = FRAME_BUFFER_WIDTH - 270.0f;
	rcStaminaBarFrame->right = FRAME_BUFFER_WIDTH - 270.0f + m_fMaxStamina;
	rcStaminaBarFrame->bottom = FRAME_BUFFER_HEIGHT - 210.0f;

	m_pd2dbrBorder->SetColor(D2D1::ColorF(D2D1::ColorF::AliceBlue, 1.0f));
	m_pd2dDeviceContext->DrawRectangle(rcStaminaBarFrame, m_pd2dbrBorder);

	delete rcStaminaBar;
	delete rcStaminaBarFrame;

}


//========================================================================================//
// Second Round

void CSecondRoundSceneUI::DrawUI(UINT m_nSwapChainBufferIndex)
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

void CSecondRoundSceneUI::UISet(UINT m_nSwapChainBufferIndex)
{
	D2D1_SIZE_F szRenderTarget = m_ppd2dRenderTargets[m_nSwapChainBufferIndex]->GetSize();

	if (m_bStaminaBarOn) StaminaBarUI();
	ItemUI();
}

void CSecondRoundSceneUI::ItemUI()
{
	D2D1_RECT_F* rcMissionBarFrame;
	rcMissionBarFrame = new D2D1_RECT_F;
	rcMissionBarFrame->top = FRAME_BUFFER_HEIGHT - 200.f;
	rcMissionBarFrame->left = FRAME_BUFFER_WIDTH - 270.0f;
	rcMissionBarFrame->right = FRAME_BUFFER_WIDTH - 50.f;
	rcMissionBarFrame->bottom = FRAME_BUFFER_HEIGHT - 50.0f;

	m_pd2dbrBorder->SetColor(D2D1::ColorF(D2D1::ColorF::GreenYellow, 1.0f));
	m_pd2dDeviceContext->DrawRectangle(rcMissionBarFrame, m_pd2dbrBorder);

	delete rcMissionBarFrame;

	float top = 1650.f;
	float left = 850.f;
	float gab = 100.f;

	D2D1_RECT_F rcUpperText = D2D1::RectF(top, left, top + gab, left + gab);
	WCHAR MissionText[] = L"ITEM";
	m_pd2dbrText->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 1.0f));
	m_pd2dDeviceContext->DrawTextW(MissionText, (UINT32)wcslen(MissionText), m_pdwFont, &rcUpperText, m_pd2dbrText);

}

void CSecondRoundSceneUI::StaminaBarUI()
{
	float halfsize = m_fMaxStamina / 2;

	//게이지 바
	D2D1_RECT_F* rcStaminaBar;
	rcStaminaBar = new D2D1_RECT_F;
	rcStaminaBar->top = FRAME_BUFFER_HEIGHT - 230.f;
	rcStaminaBar->left = FRAME_BUFFER_WIDTH - 270.0f;
	rcStaminaBar->right = FRAME_BUFFER_WIDTH - 270.0f + m_fStaminaRange;
	rcStaminaBar->bottom = FRAME_BUFFER_HEIGHT - 210.0f;

	m_pd2dbrBorder->SetColor(D2D1::ColorF(D2D1::ColorF::RoyalBlue, 1.0f));
	m_pd2dDeviceContext->FillRectangle(rcStaminaBar, m_pd2dbrBorder);

	// 게이지 바 프레임
	D2D1_RECT_F* rcStaminaBarFrame;
	rcStaminaBarFrame = new D2D1_RECT_F;
	rcStaminaBarFrame->top = FRAME_BUFFER_HEIGHT - 230.f;
	rcStaminaBarFrame->left = FRAME_BUFFER_WIDTH - 270.0f;
	rcStaminaBarFrame->right = FRAME_BUFFER_WIDTH - 270.0f + m_fMaxStamina;
	rcStaminaBarFrame->bottom = FRAME_BUFFER_HEIGHT - 210.0f;

	m_pd2dbrBorder->SetColor(D2D1::ColorF(D2D1::ColorF::AliceBlue, 1.0f));
	m_pd2dDeviceContext->DrawRectangle(rcStaminaBarFrame, m_pd2dbrBorder);

	delete rcStaminaBar;
	delete rcStaminaBarFrame;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///

CStartSceneUI::CStartSceneUI() {

	// set Button
	m_nButton = 2;
	m_ppButton = new CButton * [m_nButton];
	
	m_ppButton[0] = new CButton(0, 0, 100, 100, L"test");
	m_ppButton[1] = new CButton(0, 0, 100, 100, L"test");

	// set TextInputBox
	m_nTextInputBox = 2;
	m_ppTextInputBox = new CTextInput * [m_nTextInputBox];

	m_ppTextInputBox[0] = new CTextInput(100, 200, 100, 20, L"test");
	m_ppTextInputBox[1] = new CTextInput(100, 250, 100, 20, L"test");
}

void CStartSceneUI::DrawUI(UINT m_nSwapChainBufferIndex)
{
	m_pd2dDeviceContext->SetTarget(m_ppd2dRenderTargets[m_nSwapChainBufferIndex]);
	ID3D11Resource* ppd3dResources[] = { m_ppd3d11WrappedBackBuffers[m_nSwapChainBufferIndex] };
	m_pd3d11On12Device->AcquireWrappedResources(ppd3dResources, _countof(ppd3dResources));

	m_pd2dDeviceContext->BeginDraw();
	m_pd2dDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());

	//Direct2D Drawing
#ifdef SMALL_WINDOW_SCREEN

	UISet_Small(m_nSwapChainBufferIndex);
#else
	UISet_Full(m_nSwapChainBufferIndex);
#endif // SMALL_WINDOW_SCREEN


	m_pd2dDeviceContext->EndDraw();
	m_pd3d11On12Device->ReleaseWrappedResources(ppd3dResources, _countof(ppd3dResources));
	m_pd3d11DeviceContext->Flush();
}

void CStartSceneUI::UISet_Small(UINT m_nSwapChainBufferIndex)
{
	D2D1_SIZE_F szRenderTarget = m_ppd2dRenderTargets[m_nSwapChainBufferIndex]->GetSize();

	//=================================================
	// Background Image
	LoadUIImage(L"Image/StartBG_Small.png", m_pwicImagingFactory, m_pd2dfxBitmapSource);
	D2D_POINT_2F d2dPoint = { 0.f, 0.f };
	D2D_RECT_F d2dRect = { 0.0f, 0.0f, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
	m_pd2dDeviceContext->DrawImage(m_pd2dfxBitmapSource, &d2dPoint, &d2dRect);
	
	//=================================================
	// Partition Rect

	D2D1_RECT_F		PartitionRect = { 0.f , 0.f, 270.f, 380.f };
	m_pd2dbrBorder->SetColor(D2D1::ColorF(0x000000, 0.8f));
	m_pd2dDeviceContext->FillRectangle(PartitionRect, m_pd2dbrBorder);

	//=================================================
	//Button 

	m_ppButton[0]->SetPosition(FRAME_BUFFER_WIDTH - 600, FRAME_BUFFER_HEIGHT / 2 + 80);
	m_ppButton[0]->SetSize(80, 30);
	m_ppButton[0]->CreateTextFormat(m_pdWriteFactory, 10.f);
	m_ppButton[0]->SetText(L"Sign Up");
	m_ppButton[0]->Draw(m_pd2dDeviceContext, m_pd2dbrText, m_pd2dbrBorder);

	m_ppButton[1]->SetPosition(FRAME_BUFFER_WIDTH - 500, FRAME_BUFFER_HEIGHT / 2 + 80);
	m_ppButton[1]->SetSize(80, 30);
	m_ppButton[1]->CreateTextFormat(m_pdWriteFactory, 10.f);
	m_ppButton[1]->SetText(L"Login");
	m_ppButton[1]->Draw(m_pd2dDeviceContext, m_pd2dbrText, m_pd2dbrBorder);

	//=================================================
	// TextBox (ID/PW)
	
	//ID
	m_ppTextInputBox[0]->SetPosition(FRAME_BUFFER_WIDTH - 600, FRAME_BUFFER_HEIGHT / 2 + 30);
	m_ppTextInputBox[0]->SetSize(200, 20);
	m_ppTextInputBox[0]->CreateTextFormat(m_pdWriteFactory, 15.f);
	m_ppTextInputBox[0]->SetText(m_ID.c_str());
	m_ppTextInputBox[0]->Draw(m_pd2dDeviceContext, m_pd2dbrText, m_pd2dbrBorder);

	//PW
	m_ppTextInputBox[1]->SetPosition(FRAME_BUFFER_WIDTH - 600, FRAME_BUFFER_HEIGHT / 2 + 53);
	m_ppTextInputBox[1]->SetSize(200, 20);
	m_ppTextInputBox[1]->CreateTextFormat(m_pdWriteFactory, 15.f);
	m_ppTextInputBox[1]->SetText(m_PW.c_str());
	m_ppTextInputBox[1]->Draw(m_pd2dDeviceContext, m_pd2dbrText, m_pd2dbrBorder);


	//=================================================
	// Tiltle

	D2D1_RECT_F rcTitleRect = { 0.0f, 0.0f, 230.0f, 175.0f };
	D2D_POINT_2F lTitle_Position = { 15.f, 0.f };

	LoadUIImage(L"Image/Title_small.png", m_pwicImagingFactory, m_pd2dfxBitmapSource);
	m_pd2dDeviceContext->DrawImage(m_pd2dfxBitmapSource, &lTitle_Position, &rcTitleRect);

	// Explane text
	m_pdWriteFactory->CreateTextFormat(L"ComicSans", NULL, DWRITE_FONT_WEIGHT_DEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 15.0f, L"en-US", &m_pdwFont);

	float left = 40;
	float top = 180;
	float width = 150.f;
	float height = 50.f;

	D2D1_RECT_F rcPressText = D2D1::RectF(left, top, left + width, top + height);
	
	std::wstring SatrtInfo;

	switch (m_CheckInfo)
	{
	case EMPTY:
		SatrtInfo = L"Enter ID and PW";
		break;
	case ALL_CORRET:
		SatrtInfo = L"Press 'A' to Start";
		break;
	case ID_ERROR:
		SatrtInfo = L"ID ERROR";
		break;
	case PW_ERROR:
		SatrtInfo = L"PW ERROR";
		break;
	default:
		break;
	}
	
	m_pd2dbrText->SetColor(D2D1::ColorF(D2D1::ColorF::White, 1.0f));
	m_pd2dDeviceContext->DrawTextW(SatrtInfo.c_str(), static_cast<UINT32>(SatrtInfo.length()), m_pdwFont, &rcPressText, m_pd2dbrText);
}

void CStartSceneUI::UISet_Full(UINT m_nSwapChainBufferIndex)
{
	D2D1_SIZE_F szRenderTarget = m_ppd2dRenderTargets[m_nSwapChainBufferIndex]->GetSize();

	//=================================================
	// Background Image
	LoadUIImage(L"Image/StartBG.png", m_pwicImagingFactory, m_pd2dfxBitmapSource);
	D2D_POINT_2F d2dPoint = { 0.f, 0.f };
	D2D_RECT_F d2dRect = { 0.0f, 0.0f, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
	m_pd2dDeviceContext->DrawImage(m_pd2dfxBitmapSource, &d2dPoint, &d2dRect);

	//=================================================
	// partition Rect

	D2D1_RECT_F		PartitionRect = { 0.f , 0.f, 700.f, 1090.f };
	m_pd2dbrBorder->SetColor(D2D1::ColorF(0x000000, 0.8f));
	m_pd2dDeviceContext->FillRectangle(PartitionRect, m_pd2dbrBorder);

	//=================================================
	//Button 

	m_ppButton[0]->SetPosition(40, FRAME_BUFFER_HEIGHT / 3 + 220);
	m_ppButton[0]->SetSize(130, 50);
	m_ppButton[0]->CreateTextFormat(m_pdWriteFactory, 20.f);
	m_ppButton[0]->SetText(L"Sign Up");
	m_ppButton[0]->Draw(m_pd2dDeviceContext, m_pd2dbrText, m_pd2dbrBorder);

	m_ppButton[1]->SetPosition(200, FRAME_BUFFER_HEIGHT / 3 + 220);
	m_ppButton[1]->SetSize(130, 50);
	m_ppButton[1]->CreateTextFormat(m_pdWriteFactory, 20.f);
	m_ppButton[1]->SetText(L"Login");
	m_ppButton[1]->Draw(m_pd2dDeviceContext, m_pd2dbrText, m_pd2dbrBorder);

	//=================================================
	// TextBox (ID/PW)

	//ID
	m_ppTextInputBox[0]->SetPosition(40, FRAME_BUFFER_HEIGHT / 3 +100);
	m_ppTextInputBox[0]->SetSize(500, 40);
	m_ppTextInputBox[0]->CreateTextFormat(m_pdWriteFactory, 30.f);
	m_ppTextInputBox[0]->SetText(m_ID.c_str());
	m_ppTextInputBox[0]->Draw(m_pd2dDeviceContext, m_pd2dbrText, m_pd2dbrBorder);

	//PW
	m_ppTextInputBox[1]->SetPosition(40, FRAME_BUFFER_HEIGHT / 3 + 160);
	m_ppTextInputBox[1]->SetSize(500, 40);
	m_ppTextInputBox[1]->CreateTextFormat(m_pdWriteFactory, 30.f);
	m_ppTextInputBox[1]->SetText(m_PW.c_str());
	m_ppTextInputBox[1]->Draw(m_pd2dDeviceContext, m_pd2dbrText, m_pd2dbrBorder);


	//=================================================
	// Tiltle

	D2D1_RECT_F rcTitleRect = { 0.0f, 0.0f, 460.0f, 350.0f };
	D2D_POINT_2F lTitle_Position = { 60.f, 20.f };

	LoadUIImage(L"Image/Title.png", m_pwicImagingFactory, m_pd2dfxBitmapSource);
	m_pd2dDeviceContext->DrawImage(m_pd2dfxBitmapSource, &lTitle_Position, &rcTitleRect);


	// 로그인창 아래 안내 문구용
	m_pdWriteFactory->CreateTextFormat(L"ComicSans", NULL, DWRITE_FONT_WEIGHT_DEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 20.0f, L"en-US", &m_pdwFont);
	m_pdwFont->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	float top = FRAME_BUFFER_HEIGHT / 3 + 70;
	float left = 40;
	float width = 200.f;
	float height = 10.f;

	D2D1_RECT_F rcInfoText = D2D1::RectF(left, top, left + width, top + height);
	std::wstring SatrtInfo;
	switch (m_CheckInfo)
	{
	case EMPTY:
		SatrtInfo = L"* Enter ID and PW";
		break;
	case ALL_CORRET:
		SatrtInfo = L"* Press 'A' to Start";
		break;
	case ID_ERROR:
		SatrtInfo = L"* ID ERROR";
		break;
	case PW_ERROR:
		SatrtInfo = L"* PW ERROR";
		break;
	default:
		break;
	}

	m_pd2dbrText->SetColor(D2D1::ColorF(D2D1::ColorF::White, 1.0f));
	m_pd2dDeviceContext->DrawTextW(SatrtInfo.c_str(), static_cast<UINT32>(SatrtInfo.length()), m_pdwFont, &rcInfoText, m_pd2dbrText);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///

void CLoadingUI::DrawUI(UINT m_nSwapChainBufferIndex)
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

void CLoadingUI::UISet(UINT m_nSwapChainBufferIndex)
{
	D2D1_SIZE_F szRenderTarget = m_ppd2dRenderTargets[m_nSwapChainBufferIndex]->GetSize();

	// Background Image
	D2D1_RECT_F		PartitionRect = { 0.f , 0.f, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
	m_pd2dbrBorder->SetColor(D2D1::ColorF(0x000000, 1.0f));
	m_pd2dDeviceContext->FillRectangle(PartitionRect, m_pd2dbrBorder);


	m_pdWriteFactory->CreateTextFormat(L"ComicSans", NULL, DWRITE_FONT_WEIGHT_DEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 20.0f, L"en-US", &m_pdwFont);

	float top = FRAME_BUFFER_HEIGHT - (FRAME_BUFFER_HEIGHT / 2);
	float left = FRAME_BUFFER_WIDTH - (FRAME_BUFFER_WIDTH / 2);
	float width = 200.f;
	float height = 200.f;

	D2D1_RECT_F rcPressText = D2D1::RectF(top, left, top + height, left + width);
	WCHAR PressA[] = L"Now Loading...";
	m_pd2dbrText->SetColor(D2D1::ColorF(D2D1::ColorF::White, 1.0f));
	m_pd2dDeviceContext->DrawTextW(PressA, (UINT32)wcslen(PressA), m_pdwFont, &rcPressText, m_pd2dbrText);
}

