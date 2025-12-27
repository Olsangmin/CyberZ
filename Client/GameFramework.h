#pragma once

#ifdef SMALL_WINDOW_SCREEN

#define FRAME_BUFFER_WIDTH				1280
#define FRAME_BUFFER_HEIGHT				720 

#else

#define FRAME_BUFFER_WIDTH				1920
#define FRAME_BUFFER_HEIGHT				1080

#endif // SMALL_WINDOW_SCREEN

#include "Timer.h"
#include "Player.h"

#include "Scene.h"
#include "PlayScene.h"
#include "PrepareScene.h"
#include "StartScene.h"
#include "LoadingScene.h"


class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void CreateSwapChain();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();

	void CreateRtvAndDsvDescriptorHeaps();
	void CreateSwapChainRenderTargetViews();

	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	void ChangeSwapChainState();

    void BuildObjects(int myPlayerNum);
	void ReleaseObjects();
	
	
	void ChangeScene(SCENENUM nScene, int myPlayerNum);
	void GetSceneInfo();
	void SetSceneInfo();
	void LoadingCHK();

    void AnimateObjects();
    void FrameAdvance();

	void WaitForGpuComplete();
	void MoveToNextFrame();

	// 키 & 마우스 입력
    void ProcessInput();
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);


private:
	HINSTANCE					m_hInstance;
	HWND						m_hWnd; 

	int							m_nWndClientWidth;
	int							m_nWndClientHeight;
        
	IDXGIFactory4*				m_pdxgiFactory = NULL;
	IDXGISwapChain3*			m_pdxgiSwapChain = NULL;
	ID3D12Device*				m_pd3dDevice = NULL;

	bool						m_bMsaa4xEnable = false;
	UINT						m_nMsaa4xQualityLevels = 0;

	static const UINT			m_nSwapChainBuffers = 2;
	UINT						m_nSwapChainBufferIndex;

	ID3D12DescriptorHeap*			m_pd3dRtvDescriptorHeap = NULL;
	ID3D12Resource*					m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers];
	D3D12_CPU_DESCRIPTOR_HANDLE		m_pd3dSwapChainBackBufferRTVCPUHandles[m_nSwapChainBuffers];


	ID3D12Resource*					m_pd3dDepthStencilBuffer = NULL;
	ID3D12DescriptorHeap*			m_pd3dDsvDescriptorHeap = NULL;
	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dDsvDescriptorCPUHandle;


	ID3D12CommandAllocator*		m_pd3dCommandAllocator = NULL;
	ID3D12CommandQueue*			m_pd3dCommandQueue = NULL;
	ID3D12GraphicsCommandList*	m_pd3dCommandList = NULL;

	ID3D12Fence*				m_pd3dFence = NULL;
	UINT64						m_nFenceValues[m_nSwapChainBuffers];
	HANDLE						m_hFenceEvent;
	//CGameSound*					m_pSound;

#if defined(_DEBUG)
	ID3D12Debug*				m_pd3dDebugController;
#endif

	CGameTimer					m_GameTimer;

	CScene*						m_pScene = NULL;
	bool						m_bLoading = true;

	CCamera*					m_pCamera = NULL;

	POINT						m_ptOldCursorPos;

	bool						m_bRenderBoundingBox = false;

	_TCHAR						m_pszFrameRate[70];

	SCENENUM					m_nSceneNum = START_SCENE;	// 출력 씬 번호
	int							PlayerInfo[MAX_PLAYER];

	int							m_nDrawOption = 84;
	CPostProcessingShader		*m_pPostProcessingShader = NULL;
};

