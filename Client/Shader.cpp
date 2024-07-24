//-----------------------------------------------------------------------------
// File: Shader.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Scene.h"
#include "Shader.h"


CShader::CShader()
{
}

CShader::~CShader()
{
	ReleaseShaderVariables();

	if (m_pd3dPipelineState) m_pd3dPipelineState->Release();
}

D3D12_SHADER_BYTECODE CShader::CreateVertexShader()
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

D3D12_SHADER_BYTECODE CShader::CreatePixelShader()
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob **ppd3dShaderBlob)
{
	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob *pd3dErrorBlob = NULL;
	HRESULT hResult = ::D3DCompileFromFile(pszFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, pszShaderName, pszShaderProfile, nCompileFlags, 0, ppd3dShaderBlob, &pd3dErrorBlob);
	char *pErrorString = NULL;
	if (pd3dErrorBlob) pErrorString = (char *)pd3dErrorBlob->GetBufferPointer();

	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();

	return(d3dShaderByteCode);
}

#define _WITH_WFOPEN
//#define _WITH_STD_STREAM

#ifdef _WITH_STD_STREAM
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#endif

D3D12_SHADER_BYTECODE CShader::ReadCompiledShaderFromFile(WCHAR *pszFileName, ID3DBlob **ppd3dShaderBlob)
{
	UINT nReadBytes = 0;
#ifdef _WITH_WFOPEN
	FILE *pFile = NULL;
	::_wfopen_s(&pFile, pszFileName, L"rb");
	::fseek(pFile, 0, SEEK_END);
	int nFileSize = ::ftell(pFile);
	BYTE *pByteCode = new BYTE[nFileSize];
	::rewind(pFile);
	nReadBytes = (UINT)::fread(pByteCode, sizeof(BYTE), nFileSize, pFile);
	::fclose(pFile);
#endif
#ifdef _WITH_STD_STREAM
	std::ifstream ifsFile;
	ifsFile.open(pszFileName, std::ios::in | std::ios::ate | std::ios::binary);
	nReadBytes = (int)ifsFile.tellg();
	BYTE *pByteCode = new BYTE[*pnReadBytes];
	ifsFile.seekg(0);
	ifsFile.read((char *)pByteCode, nReadBytes);
	ifsFile.close();
#endif

	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	if (ppd3dShaderBlob)
	{
		*ppd3dShaderBlob = NULL;
		HRESULT hResult = D3DCreateBlob(nReadBytes, ppd3dShaderBlob);
		memcpy((*ppd3dShaderBlob)->GetBufferPointer(), pByteCode, nReadBytes);
		d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
		d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();
	}
	else
	{
		d3dShaderByteCode.BytecodeLength = nReadBytes;
		d3dShaderByteCode.pShaderBytecode = pByteCode;
	}

	return(d3dShaderByteCode);
}

D3D12_INPUT_LAYOUT_DESC CShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = NULL;
	d3dInputLayoutDesc.NumElements = 0;

	return(d3dInputLayoutDesc);
}

D3D12_RASTERIZER_DESC CShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	//	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}

D3D12_DEPTH_STENCIL_DESC CShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_BLEND_DESC CShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = TRUE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;



	return(d3dBlendDesc);
}

void CShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE d3dPrimitiveTopologyType)
{
	::ZeroMemory(&m_d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	m_d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	m_d3dPipelineStateDesc.VS = CreateVertexShader();
	m_d3dPipelineStateDesc.PS = CreatePixelShader();
	m_d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	m_d3dPipelineStateDesc.BlendState = CreateBlendState();
	m_d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	m_d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	m_d3dPipelineStateDesc.SampleMask = UINT_MAX;
	m_d3dPipelineStateDesc.PrimitiveTopologyType = d3dPrimitiveTopologyType;
	m_d3dPipelineStateDesc.NumRenderTargets = 1;
	m_d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	m_d3dPipelineStateDesc.SampleDesc.Count = 1;
	m_d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&m_d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void CShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, UINT nRenderTargets, DXGI_FORMAT* pdxgiRtvFormats, DXGI_FORMAT dxgiDsvFormat)
{

	::ZeroMemory(&m_d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	m_d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	m_d3dPipelineStateDesc.VS = CreateVertexShader();
	m_d3dPipelineStateDesc.PS = CreatePixelShader();
	m_d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	m_d3dPipelineStateDesc.BlendState = CreateBlendState();
	m_d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	m_d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	m_d3dPipelineStateDesc.SampleMask = UINT_MAX;
	m_d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	m_d3dPipelineStateDesc.NumRenderTargets = nRenderTargets;
	for (UINT i = 0; i < nRenderTargets; i++) m_d3dPipelineStateDesc.RTVFormats[i] = (pdxgiRtvFormats) ? pdxgiRtvFormats[i] : DXGI_FORMAT_R8G8B8A8_UNORM;
	m_d3dPipelineStateDesc.DSVFormat = dxgiDsvFormat; //DXGI_FORMAT_D24_UNORM_S8_UINT;
	m_d3dPipelineStateDesc.SampleDesc.Count = 1;
	m_d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&m_d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_pd3dPipelineState);

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;

}

void CShader::OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList, int nPipelineState)
{
	if (m_pd3dGraphicsRootSignature) pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	if (m_pd3dPipelineState) pd3dCommandList->SetPipelineState(m_pd3dPipelineState);
}

// void CShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
// {
// 	OnPrepareRender(pd3dCommandList);
// }

void CShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, void* pContext)
{
	OnPrepareRender(pd3dCommandList);
	UpdateShaderVariables(pd3dCommandList, pContext);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CTerrainShader::CTerrainShader()
{
}

CTerrainShader::~CTerrainShader()
{
}

D3D12_INPUT_LAYOUT_DESC CTerrainShader::CreateInputLayout()
{
	UINT nInputElementDescs = 4;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[3] = { "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CTerrainShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSTerrain", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CTerrainShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSTerrain", "ps_5_1", &m_pd3dPixelShaderBlob));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CSkyBoxShader::CSkyBoxShader()
{
}

CSkyBoxShader::~CSkyBoxShader()
{
}

D3D12_INPUT_LAYOUT_DESC CSkyBoxShader::CreateInputLayout()
{
	UINT nInputElementDescs = 1;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_DEPTH_STENCIL_DESC CSkyBoxShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0xff;
	d3dDepthStencilDesc.StencilWriteMask = 0xff;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_INCR;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_DECR;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CSkyBoxShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSSkyBox", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CSkyBoxShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSSkyBox", "ps_5_1", &m_pd3dPixelShaderBlob));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CStandardShader::CStandardShader()
{
}

CStandardShader::~CStandardShader()
{
}

D3D12_INPUT_LAYOUT_DESC CStandardShader::CreateInputLayout()
{
	UINT nInputElementDescs = 5;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[4] = { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CStandardShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSStandard", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CStandardShader::CreatePixelShader()
{
	//return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSStandard", "ps_5_1", &m_pd3dPixelShaderBlob));
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSTexturedLightingToMultipleRTs", "ps_5_1", &m_pd3dPixelShaderBlob));

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CSkinnedAnimationStandardShader::CSkinnedAnimationStandardShader()
{
}

CSkinnedAnimationStandardShader::~CSkinnedAnimationStandardShader()
{
}

D3D12_INPUT_LAYOUT_DESC CSkinnedAnimationStandardShader::CreateInputLayout()
{
	UINT nInputElementDescs = 7;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[4] = { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[5] = { "BONEINDEX", 0, DXGI_FORMAT_R32G32B32A32_SINT, 5, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[6] = { "BONEWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 6, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CSkinnedAnimationStandardShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSSkinnedAnimationStandard", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CSkinnedAnimationStandardShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSTexturedLightingToMultipleRTs", "ps_5_1", &m_pd3dPixelShaderBlob));

}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CStandardObjectsShader::CStandardObjectsShader()
{
}

CStandardObjectsShader::~CStandardObjectsShader()
{
}

void CStandardObjectsShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, CLoadedModelInfo *pModel, void *pContext)
{
}

void CStandardObjectsShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) m_ppObjects[j]->Release();
		delete[] m_ppObjects;
	}
}

void CStandardObjectsShader::AnimateObjects(float fTimeElapsed)
{
	m_fElapsedTime = fTimeElapsed;
}

void CStandardObjectsShader::ReleaseUploadBuffers()
{
	for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) m_ppObjects[j]->ReleaseUploadBuffers();
}

void CStandardObjectsShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CStandardShader::Render(pd3dCommandList, pCamera);

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			m_ppObjects[j]->Animate(m_fElapsedTime);
			m_ppObjects[j]->UpdateTransform(NULL);
			m_ppObjects[j]->Render(pd3dCommandList, pCamera);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CSkinnedAnimationObjectsShader::CSkinnedAnimationObjectsShader()
{
}

CSkinnedAnimationObjectsShader::~CSkinnedAnimationObjectsShader()
{
}

void CSkinnedAnimationObjectsShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, CLoadedModelInfo *pModel, void *pContext)
{
}

void CSkinnedAnimationObjectsShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) m_ppObjects[j]->Release();
		delete[] m_ppObjects;
	}
}

void CSkinnedAnimationObjectsShader::AnimateObjects(float fTimeElapsed)
{
	m_fElapsedTime = fTimeElapsed;
}

void CSkinnedAnimationObjectsShader::ReleaseUploadBuffers()
{
	for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) m_ppObjects[j]->ReleaseUploadBuffers();
}

void CSkinnedAnimationObjectsShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CSkinnedAnimationStandardShader::Render(pd3dCommandList, pCamera);

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			m_ppObjects[j]->Animate(m_fElapsedTime);
			m_ppObjects[j]->Render(pd3dCommandList, pCamera);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAngrybotObjectsShader::CAngrybotObjectsShader()
{
}

CAngrybotObjectsShader::~CAngrybotObjectsShader()
{
}

void CAngrybotObjectsShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, CLoadedModelInfo *pModel, void *pContext)
{
	int xObjects = 3, zObjects = 3, i = 0;

	m_nObjects = (xObjects * 2 + 1) * (zObjects * 2 + 1);

	m_ppObjects = new CGameObject*[m_nObjects];

	float fxPitch = 7.0f * 2.5f;
	float fzPitch = 7.0f * 2.5f;

	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	CLoadedModelInfo *pAngrybotModel = pModel;
	if (!pAngrybotModel) pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Angrybot.bin", NULL);

	int nObjects = 0;
	for (int x = -xObjects; x <= xObjects; x++)
	{
		for (int z = -zObjects; z <= zObjects; z++)
		{
			m_ppObjects[nObjects] = new CAngrybotObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pAngrybotModel, 1);
			m_ppObjects[nObjects]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, (nObjects % 2));
			m_ppObjects[nObjects]->m_pSkinnedAnimationController->SetTrackSpeed(0, (nObjects % 2) ? 0.25f : 1.0f);
			m_ppObjects[nObjects]->m_pSkinnedAnimationController->SetTrackPosition(0, (nObjects % 3) ? 0.85f : 0.0f);
			XMFLOAT3 xmf3Position = XMFLOAT3(fxPitch*x + 390.0f, 0.0f, 730.0f + fzPitch * z);
			xmf3Position.y = pTerrain->GetHeight(xmf3Position.x, xmf3Position.z);
			m_ppObjects[nObjects]->SetPosition(xmf3Position);
			m_ppObjects[nObjects++]->SetScale(2.0f, 2.0f, 2.0f);
		}
    }

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	if (!pModel && pAngrybotModel) delete pAngrybotModel;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CBoundingBoxShader::CBoundingBoxShader()
{
}

CBoundingBoxShader::~CBoundingBoxShader()
{
}

D3D12_INPUT_LAYOUT_DESC CBoundingBoxShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_RASTERIZER_DESC CBoundingBoxShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}

D3D12_DEPTH_STENCIL_DESC CBoundingBoxShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

void CBoundingBoxShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CShader::CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
}

D3D12_SHADER_BYTECODE CBoundingBoxShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSBoundingBox", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CBoundingBoxShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSBoundingBox", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void CBoundingBoxShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CPostProcessingShader::CPostProcessingShader()
{
}

CPostProcessingShader::~CPostProcessingShader()
{
	if (m_pTexture) delete m_pTexture;

	if (m_pd3dRtvCPUDescriptorHandles) delete[] m_pd3dRtvCPUDescriptorHandles;
}

D3D12_INPUT_LAYOUT_DESC CPostProcessingShader::CreateInputLayout()
{
	
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = NULL;
	d3dInputLayoutDesc.NumElements = 0;

	return(d3dInputLayoutDesc);
}

D3D12_DEPTH_STENCIL_DESC CPostProcessingShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CPostProcessingShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSPostProcessing", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CPostProcessingShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSPostProcessing", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void CPostProcessingShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, UINT nRenderTargets, DXGI_FORMAT* pdxgiRtvFormats, DXGI_FORMAT dxgiDsvFormat)
{
	m_pd3dGraphicsRootSignature = pd3dGraphicsRootSignature;
	m_pd3dGraphicsRootSignature->AddRef();

	CShader::CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature, nRenderTargets, pdxgiRtvFormats, dxgiDsvFormat);
}

void CPostProcessingShader::CreateResourcesAndRtvsSrvs(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nRenderTargets, DXGI_FORMAT* pdxgiFormats, D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle)
{
	m_pTexture = new CTexture(nRenderTargets, RESOURCE_TEXTURE2D, 0, 1);

	D3D12_CLEAR_VALUE d3dClearValue = { DXGI_FORMAT_R8G8B8A8_UNORM, { 1.0f, 0.0f, 1.0f, 1.0f } };
	for (UINT i = 0; i < nRenderTargets; i++)
	{
		d3dClearValue.Format = pdxgiFormats[i];
		m_pTexture->CreateTexture(pd3dDevice, pd3dCommandList, i, RESOURCE_TEXTURE2D, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 1, 0, pdxgiFormats[i], D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON, &d3dClearValue);
	}

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CScene::CreateShaderResourceViews(pd3dDevice, m_pTexture, 0, 16);

	D3D12_RENDER_TARGET_VIEW_DESC d3dRenderTargetViewDesc;
	d3dRenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	d3dRenderTargetViewDesc.Texture2D.MipSlice = 0;
	d3dRenderTargetViewDesc.Texture2D.PlaneSlice = 0;

	m_pd3dRtvCPUDescriptorHandles = new D3D12_CPU_DESCRIPTOR_HANDLE[nRenderTargets];

	for (UINT i = 0; i < nRenderTargets; i++)
	{
		d3dRenderTargetViewDesc.Format = pdxgiFormats[i];
		ID3D12Resource* pd3dTextureResource = m_pTexture->GetResource(i);
		pd3dDevice->CreateRenderTargetView(pd3dTextureResource, &d3dRenderTargetViewDesc, d3dRtvCPUDescriptorHandle);
		m_pd3dRtvCPUDescriptorHandles[i] = d3dRtvCPUDescriptorHandle;
		d3dRtvCPUDescriptorHandle.ptr += ::gnRtvDescriptorIncrementSize;
	}


}

void CPostProcessingShader::OnPrepareRenderTarget(ID3D12GraphicsCommandList* pd3dCommandList, int nRenderTargets, D3D12_CPU_DESCRIPTOR_HANDLE* pd3dRtvCPUHandles, D3D12_CPU_DESCRIPTOR_HANDLE* pd3dDsvCPUHandle)
{
	int nResources = m_pTexture->GetTextures();
	D3D12_CPU_DESCRIPTOR_HANDLE* pd3dAllRtvCPUHandles = new D3D12_CPU_DESCRIPTOR_HANDLE[nRenderTargets + nResources];

	for (int i = 0; i < nRenderTargets; i++)
	{
		pd3dAllRtvCPUHandles[i] = pd3dRtvCPUHandles[i];
		pd3dCommandList->ClearRenderTargetView(pd3dRtvCPUHandles[i], Colors::White, 0, NULL);

	}

	for (int i = 0; i < nResources; i++)
	{
		::SynchronizeResourceTransition(pd3dCommandList, GetTextureResource(i), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET);

		D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = GetRtvCPUDescriptorHandle(i);
		pd3dCommandList->ClearRenderTargetView(d3dRtvCPUDescriptorHandle, Colors::White, 0, NULL);
		pd3dAllRtvCPUHandles[nRenderTargets + i] = d3dRtvCPUDescriptorHandle;
	}
	pd3dCommandList->OMSetRenderTargets(nRenderTargets + nResources, pd3dAllRtvCPUHandles, FALSE, pd3dDsvCPUHandle);

	if (pd3dAllRtvCPUHandles) delete[] pd3dAllRtvCPUHandles;
}

void CPostProcessingShader::OnPostRenderTarget(ID3D12GraphicsCommandList* pd3dCommandList)
{
	int nResources = m_pTexture->GetTextures();
	for (int i = 0; i < nResources; i++)
	{
		::SynchronizeResourceTransition(pd3dCommandList, GetTextureResource(i), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON);
	}
}

void CPostProcessingShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, void* pContext)
{
	CShader::Render(pd3dCommandList, pCamera, pContext);

	if (m_pTexture) m_pTexture->UpdateShaderVariables(pd3dCommandList);

	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pd3dCommandList->DrawInstanced(6, 1, 0, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

CTextureDeferdShader::CTextureDeferdShader()
{
}

CTextureDeferdShader::~CTextureDeferdShader()
{
	ReleaseShaderVariables();
}

D3D12_SHADER_BYTECODE CTextureDeferdShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSScreenRectSamplingTextured", "vs_5_1", &m_pd3dVertexShaderBlob));

}

D3D12_SHADER_BYTECODE CTextureDeferdShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSScreenRectSamplingTextured", "ps_5_1", &m_pd3dPixelShaderBlob));

}

void CTextureDeferdShader::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(PS_CB_DRAW_OPTIONS) + 255) & ~255); //256의 배수
	m_pd3dcbDrawOptions = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbDrawOptions->Map(0, NULL, (void**)&m_pcbMappedDrawOptions);

	CPostProcessingShader::CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CTextureDeferdShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
	m_pcbMappedDrawOptions->m_xmn4DrawOptions.x = *((int*)pContext);
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbDrawOptions->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(15, d3dGpuVirtualAddress);

	CPostProcessingShader::UpdateShaderVariables(pd3dCommandList, pContext);
}

void CTextureDeferdShader::ReleaseShaderVariables()
{
	if (m_pd3dcbDrawOptions) m_pd3dcbDrawOptions->Release();
}

void CTextureDeferdShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, void* pContext)
{
	CPostProcessingShader::Render(pd3dCommandList, pCamera, pContext);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CDepthRenderShader::CDepthRenderShader(LIGHT* pLights)
{
	m_pLights = pLights;
	m_pToLightSpaces = new TOLIGHTSPACEINFO;

	XMFLOAT4X4 xmf4x4ToTexture = { 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.0f, 1.0f };
	m_xmProjectionToTexture = XMLoadFloat4x4(&xmf4x4ToTexture);
}

CDepthRenderShader::~CDepthRenderShader()
{
	if (m_pToLightSpaces) delete m_pToLightSpaces;
}

D3D12_SHADER_BYTECODE CDepthRenderShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSDepthWriteShader", "ps_5_1", &m_pd3dPixelShaderBlob));
}

D3D12_DEPTH_STENCIL_DESC CDepthRenderShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS; //D3D12_COMPARISON_FUNC_LESS_EQUAL
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_RASTERIZER_DESC CDepthRenderShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
#ifdef _WITH_RASTERIZER_DEPTH_BIAS
	d3dRasterizerDesc.DepthBias = 250000;
#endif
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 1.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}

void CDepthRenderShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{

	// RTV 디스크립터 힙 생성
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = MAX_DEPTH_TEXTURES;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	HRESULT hResult = pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dRtvDescriptorHeap);

	// 조명 텍스처에서 깊이 텍스처 만들기
	m_pDepthFromLightTexture = new CTexture(MAX_DEPTH_TEXTURES, RESOURCE_TEXTURE2D_ARRAY, 0, 1);

	D3D12_CLEAR_VALUE d3dClearValue = { DXGI_FORMAT_R32_FLOAT, { 1.0f, 1.0f, 1.0f, 1.0f } };
	for (UINT i = 0; i < MAX_DEPTH_TEXTURES; i++) m_pDepthFromLightTexture->CreateTexture(pd3dDevice, pd3dCommandList, i, RESOURCE_TEXTURE2D, _DEPTH_BUFFER_WIDTH, _DEPTH_BUFFER_HEIGHT, 1, 0, DXGI_FORMAT_R32_FLOAT, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON, &d3dClearValue);

	// RTV 설정
	D3D12_RENDER_TARGET_VIEW_DESC d3dRenderTargetViewDesc;
	d3dRenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	d3dRenderTargetViewDesc.Texture2D.MipSlice = 0;
	d3dRenderTargetViewDesc.Texture2D.PlaneSlice = 0;
	d3dRenderTargetViewDesc.Format = DXGI_FORMAT_R32_FLOAT;

	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < MAX_DEPTH_TEXTURES; i++)
	{
		ID3D12Resource* pd3dTextureResource = m_pDepthFromLightTexture->GetResource(i);
		pd3dDevice->CreateRenderTargetView(pd3dTextureResource, &d3dRenderTargetViewDesc, d3dRtvCPUDescriptorHandle);
		m_pd3dRtvCPUDescriptorHandles[i] = d3dRtvCPUDescriptorHandle;
		d3dRtvCPUDescriptorHandle.ptr += ::gnRtvDescriptorIncrementSize;
	}

	// DSV 디스크립터 생성
	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult = pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dDsvDescriptorHeap);

	// 깊이 버퍼 리소스 생성
	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = _DEPTH_BUFFER_WIDTH;
	d3dResourceDesc.Height = _DEPTH_BUFFER_HEIGHT;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D32_FLOAT;
	d3dResourceDesc.SampleDesc.Count = 1;
	d3dResourceDesc.SampleDesc.Quality = 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask = 1;
	d3dHeapProperties.VisibleNodeMask = 1;

	d3dClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;

	pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue, __uuidof(ID3D12Resource), (void**)&m_pd3dDepthBuffer);

	// DSV 설정
	D3D12_DEPTH_STENCIL_VIEW_DESC d3dDepthStencilViewDesc;
	::ZeroMemory(&d3dDepthStencilViewDesc, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
	d3dDepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	d3dDepthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	d3dDepthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;

	m_d3dDsvDescriptorCPUHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	pd3dDevice->CreateDepthStencilView(m_pd3dDepthBuffer, &d3dDepthStencilViewDesc, m_d3dDsvDescriptorCPUHandle);

	// 카메라 설정
	for (int i = 0; i < MAX_DEPTH_TEXTURES; i++)
	{
		m_ppDepthRenderCameras[i] = new CCamera();
		m_ppDepthRenderCameras[i]->SetViewport(0, 0, _DEPTH_BUFFER_WIDTH, _DEPTH_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_ppDepthRenderCameras[i]->SetScissorRect(0, 0, _DEPTH_BUFFER_WIDTH, _DEPTH_BUFFER_HEIGHT);
		m_ppDepthRenderCameras[i]->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	}

	// CBV/SRV 디스크립터 힙 및 셰이더 리소스뷰 생성
	CScene::CreateShaderResourceViews(pd3dDevice, m_pDepthFromLightTexture, 0, 17); //Depth Buffer

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CDepthRenderShader::ReleaseObjects()
{
	for (int i = 0; i < MAX_DEPTH_TEXTURES; i++)
	{
		if (m_ppDepthRenderCameras[i])
		{
			m_ppDepthRenderCameras[i]->ReleaseShaderVariables();
			delete m_ppDepthRenderCameras[i];
		}
	}

	if (m_pDepthFromLightTexture) m_pDepthFromLightTexture->Release();
	if (m_pd3dDepthBuffer) m_pd3dDepthBuffer->Release();

	if (m_pd3dRtvDescriptorHeap) m_pd3dRtvDescriptorHeap->Release();
	if (m_pd3dDsvDescriptorHeap) m_pd3dDsvDescriptorHeap->Release();
}

void CDepthRenderShader::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbDepthElementBytes;

	ncbDepthElementBytes = ((sizeof(TOLIGHTSPACEINFO) + 255) & ~255); //256의 배수
	m_pd3dcbToLightSpaces = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbDepthElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbToLightSpaces->Map(0, NULL, (void**)&m_pcbMappedToLightSpaces);
}

void CDepthRenderShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	::memcpy(m_pcbMappedToLightSpaces, m_pToLightSpaces, sizeof(TOLIGHTSPACEINFO));

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbToLightGpuVirtualAddress = m_pd3dcbToLightSpaces->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(18, d3dcbToLightGpuVirtualAddress); // cbToLightSpace (b6)
}

void CDepthRenderShader::ReleaseShaderVariables()
{
	if (m_pd3dcbToLightSpaces)
	{
		m_pd3dcbToLightSpaces->Unmap(0, NULL);
		m_pd3dcbToLightSpaces->Release();
	}
}

//프러스텀 컬링용
//일단 꺼둠
//여기
/*
void CreateFrustumPoints(XMMATRIX& xmmtxProjection, XMVECTOR* pxmvCornerPoints)
{
	BoundingFrustum xmFrustrum(xmmtxProjection);

	static const XMVECTORU32 vGrabY = { 0x00000000,0xFFFFFFFF,0x00000000,0x00000000 };
	static const XMVECTORU32 vGrabX = { 0xFFFFFFFF,0x00000000,0x00000000,0x00000000 };

	XMVECTORF32 vRightTop = { xmFrustrum.RightSlope, xmFrustrum.TopSlope, 1.0f, 1.0f };
	XMVECTORF32 vLeftBottom = { xmFrustrum.LeftSlope, xmFrustrum.BottomSlope, 1.0f, 1.0f };
	XMVECTORF32 vNear = { xmFrustrum.Near, xmFrustrum.Near, xmFrustrum.Near, 1.0f };
	XMVECTORF32 vFar = { xmFrustrum.Far, xmFrustrum.Far, xmFrustrum.Far, 1.0f };
	XMVECTOR vRightTopNear = XMVectorMultiply(vRightTop, vNear);
	XMVECTOR vRightTopFar = XMVectorMultiply(vRightTop, vFar);
	XMVECTOR vLeftBottomNear = XMVectorMultiply(vLeftBottom, vNear);
	XMVECTOR vLeftBottomFar = XMVectorMultiply(vLeftBottom, vFar);

	pxmvCornerPoints[0] = vRightTopNear;
	pxmvCornerPoints[1] = XMVectorSelect(vRightTopNear, vLeftBottomNear, vGrabX);
	pxmvCornerPoints[2] = vLeftBottomNear;
	pxmvCornerPoints[3] = XMVectorSelect(vRightTopNear, vLeftBottomNear, vGrabY);

	pxmvCornerPoints[4] = vRightTopFar;
	pxmvCornerPoints[5] = XMVectorSelect(vRightTopFar, vLeftBottomFar, vGrabX);
	pxmvCornerPoints[6] = vLeftBottomFar;
	pxmvCornerPoints[7] = XMVectorSelect(vRightTopFar, vLeftBottomFar, vGrabY);
}
*/

struct TRIANGLECULLING
{
	XMVECTOR pt[3];
	BOOL culled;
};

void ComputeOrthographicProjectionNearAndFar(float& fNear, float& fFar, FXMVECTOR xmvLightOrthographicMin, FXMVECTOR xmvLightOrthographicMax, XMVECTOR* pxmvSceneAABBLightSpacePoints)
{
	fNear = FLT_MAX;
	fFar = -FLT_MAX;

	TRIANGLECULLING pxmvSceneAABBTriangles[16];
	int nTriangles = 1;

	static const int pnTriangleIndices[36] = { 0,1,2, 1,2,3, 4,5,6, 5,6,7, 0,2,4, 2,4,6, 1,3,5, 3,5,7, 0,1,4, 1,4,5, 2,3,6, 3,6,7 };
	int nPointPassesCollision[3];

	float fxOrthographicMin = XMVectorGetX(xmvLightOrthographicMin);
	float fxOrthographicMax = XMVectorGetX(xmvLightOrthographicMax);
	float fyOrthographicMin = XMVectorGetY(xmvLightOrthographicMin);
	float fyOrthographicMax = XMVectorGetY(xmvLightOrthographicMax);

	for (int i = 0; i < 12; i++)
	{
		pxmvSceneAABBTriangles[0].pt[0] = pxmvSceneAABBLightSpacePoints[pnTriangleIndices[i * 3 + 0]];
		pxmvSceneAABBTriangles[0].pt[1] = pxmvSceneAABBLightSpacePoints[pnTriangleIndices[i * 3 + 1]];
		pxmvSceneAABBTriangles[0].pt[2] = pxmvSceneAABBLightSpacePoints[pnTriangleIndices[i * 3 + 2]];
		pxmvSceneAABBTriangles[0].culled = false;

		nTriangles = 1;

		for (int j = 0; j < 4; j++)
		{
			float fEdge;
			int nComponent;

			if (j == 0)
			{
				fEdge = fxOrthographicMin;
				nComponent = 0;
			}
			else if (j == 1)
			{
				fEdge = fxOrthographicMax;
				nComponent = 0;
			}
			else if (j == 2)
			{
				fEdge = fyOrthographicMin;
				nComponent = 1;
			}
			else
			{
				fEdge = fyOrthographicMax;
				nComponent = 1;
			}

			for (int k = 0; k < nTriangles; k++)
			{
				if (!pxmvSceneAABBTriangles[k].culled)
				{
					int nInsideVertices = 0;
					XMVECTOR tempOrder;

					if (j == 0)
					{
						for (int m = 0; m < 3; m++)
						{
							nPointPassesCollision[m] = (XMVectorGetX(pxmvSceneAABBTriangles[k].pt[m]) > XMVectorGetX(xmvLightOrthographicMin)) ? 1 : 0;
							nInsideVertices += nPointPassesCollision[m];
						}
					}
					else if (j == 1)
					{
						for (int m = 0; m < 3; m++)
						{
							nPointPassesCollision[m] = (XMVectorGetX(pxmvSceneAABBTriangles[k].pt[m]) < XMVectorGetX(xmvLightOrthographicMax)) ? 1 : 0;
							nInsideVertices += nPointPassesCollision[m];
						}
					}
					else if (j == 2)
					{
						for (int m = 0; m < 3; m++)
						{
							nPointPassesCollision[m] = (XMVectorGetY(pxmvSceneAABBTriangles[k].pt[m]) > XMVectorGetY(xmvLightOrthographicMin)) ? 1 : 0;
							nInsideVertices += nPointPassesCollision[m];
						}
					}
					else
					{
						for (int m = 0; m < 3; m++)
						{
							nPointPassesCollision[m] = (XMVectorGetY(pxmvSceneAABBTriangles[k].pt[m]) < XMVectorGetY(xmvLightOrthographicMax)) ? 1 : 0;
							nInsideVertices += nPointPassesCollision[m];
						}
					}

					if (nPointPassesCollision[1] && !nPointPassesCollision[0])
					{
						tempOrder = pxmvSceneAABBTriangles[k].pt[0];
						pxmvSceneAABBTriangles[k].pt[0] = pxmvSceneAABBTriangles[k].pt[1];
						pxmvSceneAABBTriangles[k].pt[1] = tempOrder;
						nPointPassesCollision[0] = TRUE;
						nPointPassesCollision[1] = FALSE;
					}
					if (nPointPassesCollision[2] && !nPointPassesCollision[1])
					{
						tempOrder = pxmvSceneAABBTriangles[k].pt[1];
						pxmvSceneAABBTriangles[k].pt[1] = pxmvSceneAABBTriangles[k].pt[2];
						pxmvSceneAABBTriangles[k].pt[2] = tempOrder;
						nPointPassesCollision[1] = TRUE;
						nPointPassesCollision[2] = FALSE;
					}
					if (nPointPassesCollision[1] && !nPointPassesCollision[0])
					{
						tempOrder = pxmvSceneAABBTriangles[k].pt[0];
						pxmvSceneAABBTriangles[k].pt[0] = pxmvSceneAABBTriangles[k].pt[1];
						pxmvSceneAABBTriangles[k].pt[1] = tempOrder;
						nPointPassesCollision[0] = TRUE;
						nPointPassesCollision[1] = FALSE;
					}

					if (nInsideVertices == 0)
					{
						pxmvSceneAABBTriangles[k].culled = true;
					}
					else if (nInsideVertices == 1)
					{
						pxmvSceneAABBTriangles[k].culled = false;

						XMVECTOR vVert0ToVert1 = pxmvSceneAABBTriangles[k].pt[1] - pxmvSceneAABBTriangles[k].pt[0];
						XMVECTOR vVert0ToVert2 = pxmvSceneAABBTriangles[k].pt[2] - pxmvSceneAABBTriangles[k].pt[0];

						float fHitPointTimeRatio = fEdge - XMVectorGetByIndex(pxmvSceneAABBTriangles[k].pt[0], nComponent);
						float fDistanceAlongVector01 = fHitPointTimeRatio / XMVectorGetByIndex(vVert0ToVert1, nComponent);
						float fDistanceAlongVector02 = fHitPointTimeRatio / XMVectorGetByIndex(vVert0ToVert2, nComponent);
						vVert0ToVert1 *= fDistanceAlongVector01;
						vVert0ToVert1 += pxmvSceneAABBTriangles[k].pt[0];
						vVert0ToVert2 *= fDistanceAlongVector02;
						vVert0ToVert2 += pxmvSceneAABBTriangles[k].pt[0];

						pxmvSceneAABBTriangles[k].pt[1] = vVert0ToVert2;
						pxmvSceneAABBTriangles[k].pt[2] = vVert0ToVert1;
					}
					else if (nInsideVertices == 2)
					{
						pxmvSceneAABBTriangles[nTriangles] = pxmvSceneAABBTriangles[k + 1];

						pxmvSceneAABBTriangles[k].culled = false;
						pxmvSceneAABBTriangles[k + 1].culled = false;

						XMVECTOR vVert2ToVert0 = pxmvSceneAABBTriangles[k].pt[0] - pxmvSceneAABBTriangles[k].pt[2];
						XMVECTOR vVert2ToVert1 = pxmvSceneAABBTriangles[k].pt[1] - pxmvSceneAABBTriangles[k].pt[2];

						float fHitPointTime_2_0 = fEdge - XMVectorGetByIndex(pxmvSceneAABBTriangles[k].pt[2], nComponent);
						float fDistanceAlongVector_2_0 = fHitPointTime_2_0 / XMVectorGetByIndex(vVert2ToVert0, nComponent);
						vVert2ToVert0 *= fDistanceAlongVector_2_0;
						vVert2ToVert0 += pxmvSceneAABBTriangles[k].pt[2];

						pxmvSceneAABBTriangles[k + 1].pt[0] = pxmvSceneAABBTriangles[k].pt[0];
						pxmvSceneAABBTriangles[k + 1].pt[1] = pxmvSceneAABBTriangles[k].pt[1];
						pxmvSceneAABBTriangles[k + 1].pt[2] = vVert2ToVert0;

						float fHitPointTime_2_1 = fEdge - XMVectorGetByIndex(pxmvSceneAABBTriangles[k].pt[2], nComponent);
						float fDistanceAlongVector_2_1 = fHitPointTime_2_1 / XMVectorGetByIndex(vVert2ToVert1, nComponent);
						vVert2ToVert1 *= fDistanceAlongVector_2_1;
						vVert2ToVert1 += pxmvSceneAABBTriangles[k].pt[2];
						pxmvSceneAABBTriangles[k].pt[0] = pxmvSceneAABBTriangles[k + 1].pt[1];
						pxmvSceneAABBTriangles[k].pt[1] = pxmvSceneAABBTriangles[k + 1].pt[2];
						pxmvSceneAABBTriangles[k].pt[2] = vVert2ToVert1;

						++nTriangles;
						++k;
					}
					else
					{
						pxmvSceneAABBTriangles[k].culled = false;
					}
				}
			}
		}
		for (int n = 0; n < nTriangles; n++)
		{
			if (!pxmvSceneAABBTriangles[n].culled)
			{
				for (int x = 0; x < 3; x++)
				{
					float z = XMVectorGetZ(pxmvSceneAABBTriangles[n].pt[x]);
					if (fNear > z) fNear = z;
					if (fFar < z) fFar = z;
				}
			}
		}
	}
}

#define _WITH_ORTHOGRAPHIC_PROJECT_SCENE
//#define _WITH_ORTHOGRAPHIC_PROJECT_CAMERA_FRUSTUM
//#define _WITH_ORTHOGRAPHIC_PROJECT_CAMERA_FRUSTUM_SCENE

XMMATRIX CreateOrthographicProjectionMatrix(XMMATRIX& xmmtxLightView, CCamera* pSceneCamera, BoundingBox* pxmSceneBoundingBox)
{
	XMMATRIX xmmtxProjection;

#ifdef _WITH_ORTHOGRAPHIC_PROJECT_SCENE
	XMFLOAT3 pxmf3SceneAABBPoints[8];
	pxmSceneBoundingBox->GetCorners(pxmf3SceneAABBPoints);

	XMVECTOR xmvLightSpaceSceneAABBMin = g_XMFltMax;
	XMVECTOR xmvLightSpaceSceneAABBMax = g_XMFltMin;

	XMVECTOR pxmvLightSpaceSceneAABBPoints[8];
	for (int i = 0; i < 8; i++)
	{
		XMFLOAT4 xmf4SceneAABBPoint = XMFLOAT4(pxmf3SceneAABBPoints[i].x, pxmf3SceneAABBPoints[i].y, pxmf3SceneAABBPoints[i].z, 1.0f);
		pxmvLightSpaceSceneAABBPoints[i] = XMVector4Transform(XMLoadFloat4(&xmf4SceneAABBPoint), xmmtxLightView);
		xmvLightSpaceSceneAABBMin = XMVectorMin(pxmvLightSpaceSceneAABBPoints[i], xmvLightSpaceSceneAABBMin);
		xmvLightSpaceSceneAABBMax = XMVectorMax(pxmvLightSpaceSceneAABBPoints[i], xmvLightSpaceSceneAABBMax);
	}

	float fNearPlaneDistance = XMVectorGetZ(xmvLightSpaceSceneAABBMin);
	float fFarPlaneDistance = XMVectorGetZ(xmvLightSpaceSceneAABBMax);
	xmmtxProjection = XMMatrixOrthographicOffCenterLH(XMVectorGetX(xmvLightSpaceSceneAABBMin), XMVectorGetX(xmvLightSpaceSceneAABBMax), XMVectorGetY(xmvLightSpaceSceneAABBMin), XMVectorGetY(xmvLightSpaceSceneAABBMax), fNearPlaneDistance, fFarPlaneDistance);
#endif

#ifdef _WITH_ORTHOGRAPHIC_PROJECT_CAMERA_FRUSTUM
	XMFLOAT3 pxmf3CameraFrustumPoints[8];
	BoundingFrustum xmFrustrum(XMLoadFloat4x4(&pSceneCamera->m_xmf4x4Projection));
	xmFrustrum.GetCorners(pxmf3CameraFrustumPoints);

	XMMATRIX xmmtxInverseCameraView = XMMatrixInverse(NULL, XMLoadFloat4x4(&pSceneCamera->m_xmf4x4View));

	XMVECTOR xmvLightSpaceCameraFrustumAABBMin = g_XMFltMax;
	XMVECTOR xmvLightSpaceCameraFrustumAABBMax = g_XMFltMin;
	XMVECTOR xmvLightSpaceCameraFrustumPoint;
	XMVECTOR pxmvCameraFrustumPoints[8];
	for (int i = 0; i < 8; i++)
	{
		XMFLOAT4 xmf4CameraFrustumPoint = XMFLOAT4(pxmf3CameraFrustumPoints[i].x, pxmf3CameraFrustumPoints[i].y, pxmf3CameraFrustumPoints[i].z, 1.0f);
		pxmvCameraFrustumPoints[i] = XMLoadFloat4(&xmf4CameraFrustumPoint); //Camera Space
		xmvLightSpaceCameraFrustumPoint = XMVector4Transform(XMVector4Transform(pxmvCameraFrustumPoints[i], xmmtxInverseCameraView), xmmtxLightView); //Camera Space to World space, World space to Light Space
		xmvLightSpaceCameraFrustumAABBMin = XMVectorMin(xmvLightSpaceCameraFrustumPoint, xmvLightSpaceCameraFrustumAABBMin);
		xmvLightSpaceCameraFrustumAABBMax = XMVectorMax(xmvLightSpaceCameraFrustumPoint, xmvLightSpaceCameraFrustumAABBMax);
	}

	float fNearPlaneDistance = XMVectorGetZ(xmvLightSpaceCameraFrustumAABBMin);
	float fFarPlaneDistance = XMVectorGetZ(xmvLightSpaceCameraFrustumAABBMax);
	xmmtxProjection = XMMatrixOrthographicOffCenterLH(XMVectorGetX(xmvLightSpaceCameraFrustumAABBMin), XMVectorGetX(xmvLightSpaceCameraFrustumAABBMax), XMVectorGetY(xmvLightSpaceCameraFrustumAABBMin), XMVectorGetY(xmvLightSpaceCameraFrustumAABBMax), fNearPlaneDistance, fFarPlaneDistance);
#endif

#ifdef _WITH_ORTHOGRAPHIC_PROJECT_CAMERA_FRUSTUM_SCENE
	/*Fit to Scene*/
	XMFLOAT3 pxmf3CameraFrustumPoints[8];
	BoundingFrustum xmFrustrum(XMLoadFloat4x4(&pSceneCamera->GetProjectionMatrix()));
	xmFrustrum.GetCorners(pxmf3CameraFrustumPoints);

	XMMATRIX xmmtxInverseCameraView = XMMatrixInverse(NULL, XMLoadFloat4x4(&pSceneCamera->GetViewMatrix()));

	XMVECTOR xmvLightSpaceCameraFrustumAABBMin = g_XMFltMax;
	XMVECTOR xmvLightSpaceCameraFrustumAABBMax = g_XMFltMin;
	XMVECTOR xmvLightSpaceCameraFrustumPoint;
	XMVECTOR pxmvCameraFrustumPoints[8];
	for (int i = 0; i < 8; i++)
	{
		XMFLOAT4 xmf4CameraFrustumPoint = XMFLOAT4(pxmf3CameraFrustumPoints[i].x, pxmf3CameraFrustumPoints[i].y, pxmf3CameraFrustumPoints[i].z, 1.0f);
		pxmvCameraFrustumPoints[i] = XMLoadFloat4(&xmf4CameraFrustumPoint); //Camera Space
		xmvLightSpaceCameraFrustumPoint = XMVector4Transform(XMVector4Transform(pxmvCameraFrustumPoints[i], xmmtxInverseCameraView), xmmtxLightView); //Camera Space to World space, World space to Light Space
		xmvLightSpaceCameraFrustumAABBMin = XMVectorMin(xmvLightSpaceCameraFrustumPoint, xmvLightSpaceCameraFrustumAABBMin);
		xmvLightSpaceCameraFrustumAABBMax = XMVectorMax(xmvLightSpaceCameraFrustumPoint, xmvLightSpaceCameraFrustumAABBMax);
	}

	XMVECTOR xmvLightOrthographicMin = xmvLightSpaceCameraFrustumAABBMin;
	XMVECTOR xmvLightOrthographicMax = xmvLightSpaceCameraFrustumAABBMax;

	XMVECTOR xmvDiagonal = XMVector3Length(pxmvCameraFrustumPoints[0] - pxmvCameraFrustumPoints[6]);
	float fBound = XMVectorGetX(xmvDiagonal);
	XMVECTOR xmvBoarderOffset = (xmvDiagonal - (xmvLightSpaceCameraFrustumAABBMax - xmvLightSpaceCameraFrustumAABBMin)) * g_XMOneHalf;
	XMVectorSetZ(xmvBoarderOffset, 0.0f);
	XMVectorSetW(xmvBoarderOffset, 0.0f);
	xmvLightOrthographicMax += xmvBoarderOffset;
	xmvLightOrthographicMin -= xmvBoarderOffset;

	float fLightCameraOrthographicMinZ = XMVectorGetZ(xmvLightOrthographicMin);

	///*Move Light Texel Size
	XMVECTOR xmvUnitsPerTexel = g_XMZero;
	float fUnitsPerTexel = fBound / (float)_DEPTH_BUFFER_HEIGHT;
	xmvUnitsPerTexel = XMVectorSet(fUnitsPerTexel, fUnitsPerTexel, 0.0f, 0.0f);

	xmvLightOrthographicMin /= xmvUnitsPerTexel;
	xmvLightOrthographicMin = XMVectorFloor(xmvLightOrthographicMin);
	xmvLightOrthographicMin *= xmvUnitsPerTexel;

	xmvLightOrthographicMax /= xmvUnitsPerTexel;
	xmvLightOrthographicMax = XMVectorFloor(xmvLightOrthographicMax);
	xmvLightOrthographicMax *= xmvUnitsPerTexel;
	//*/

	pxmf3SceneAABBPoints[8];
	pxmSceneBoundingBox->GetCorners(pxmf3SceneAABBPoints);

	pxmvLightSpaceSceneAABBPoints[8];
	for (int i = 0; i < 8; i++)
	{
		XMFLOAT4 xmf4SceneAABBPoint = XMFLOAT4(pxmf3SceneAABBPoints[i].x, pxmf3SceneAABBPoints[i].y, pxmf3SceneAABBPoints[i].z, 1.0f);
		pxmvLightSpaceSceneAABBPoints[i] = XMVector4Transform(XMLoadFloat4(&xmf4SceneAABBPoint), xmmtxLightView);
	}

	fNearPlaneDistance = 0.0f;
	fFarPlaneDistance = 10000.0f;

	ComputeOrthographicProjectionNearAndFar(fNearPlaneDistance, fFarPlaneDistance, xmvLightOrthographicMin, xmvLightOrthographicMax, pxmvLightSpaceSceneAABBPoints);
	if (fLightCameraOrthographicMinZ > fNearPlaneDistance) fNearPlaneDistance = fLightCameraOrthographicMinZ; //Pan Caking

	xmmtxProjection = XMMatrixOrthographicOffCenterLH(XMVectorGetX(xmvLightOrthographicMin), XMVectorGetX(xmvLightOrthographicMax), XMVectorGetY(xmvLightOrthographicMin), XMVectorGetY(xmvLightOrthographicMax), fNearPlaneDistance, fFarPlaneDistance);
#endif

	return(xmmtxProjection);
}

void CDepthRenderShader::PrepareShadowMap(BoundingOrientedBox* pBoundingBoxs, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	//BoundingOrientedBox* xmpBoundingBox = 0;//= pBoundingBoxs;

	BoundingBox xmBoundingBox;
	xmBoundingBox.Center = XMFLOAT3(500.0, 10.0, 500.0);


	for (int j = 0; j < 1; j++)
	{
		if (m_pLights[j].m_bEnable)
		{
			XMFLOAT3 xmf3Position = m_pLights[j].m_xmf3Position;
			XMFLOAT3 xmf3Look = XMFLOAT3(0.0f, 1.0f, 0.0f);
			XMFLOAT3 xmf3Up = XMFLOAT3(0.0f, +1.0f, 0.0f);

			if(m_pLights[j].m_bEnable) xmf3Look = m_pLights[j].m_xmf3Direction;

			XMMATRIX xmmtxLightView = XMMatrixLookToLH(XMLoadFloat3(&xmf3Position), XMLoadFloat3(&xmf3Look), XMLoadFloat3(&xmf3Up));

			XMMATRIX xmmtxProjection = XMMatrixIdentity();
			if (m_pLights[j].m_nType == DIRECTIONAL_LIGHT)
			{
				xmmtxProjection = CreateOrthographicProjectionMatrix(xmmtxLightView, pCamera, &xmBoundingBox);
			}
			else if (m_pLights[j].m_nType == SPOT_LIGHT)
			{
				float fFovAngle = 60.0f; // m_pLights->m_pLights[j].m_fPhi = cos(60.0f);
				float fAspectRatio = float(_DEPTH_BUFFER_WIDTH) / float(_DEPTH_BUFFER_HEIGHT);
				float fNearPlaneDistance = 10.0f, fFarPlaneDistance = m_pLights[j].m_fRange;

				xmmtxProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(fFovAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
			}
			else if (m_pLights[j].m_nType == POINT_LIGHT)
			{
				//ShadowMap[6]
				//xmmtxProjection = XMMatrixIdentity();
			}

			// 조명의 내용을 카메라에 저장
			m_ppDepthRenderCameras[j]->SetPosition(xmf3Position);
			XMStoreFloat4x4(&m_ppDepthRenderCameras[j]->GetViewMatrix(), xmmtxLightView);
			XMStoreFloat4x4(&m_ppDepthRenderCameras[j]->GetProjectionMatrix(), xmmtxProjection);

			//
			XMMATRIX xmmtxToTexture = XMMatrixTranspose(xmmtxLightView * xmmtxProjection * m_xmProjectionToTexture);
			XMStoreFloat4x4(&m_pToLightSpaces->m_pxmf4x4ToTextures[j], xmmtxToTexture);
			m_pToLightSpaces->m_pxmf4LightPositions[j] = XMFLOAT4(xmf3Position.x, xmf3Position.y, xmf3Position.z, 1.0f);

			::SynchronizeResourceTransition(pd3dCommandList, m_pDepthFromLightTexture->GetResource(j), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET);

			// 렌더타겟 클리어
			float pfClearColor[4] = { 0.0f, 1.0f, 1.0f, 1.0f };
			pd3dCommandList->ClearRenderTargetView(m_pd3dRtvCPUDescriptorHandles[j], pfClearColor, 0, NULL);
			pd3dCommandList->ClearDepthStencilView(m_d3dDsvDescriptorCPUHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, NULL);
			pd3dCommandList->OMSetRenderTargets(1, &m_pd3dRtvCPUDescriptorHandles[j], TRUE, &m_d3dDsvDescriptorCPUHandle);


			Render(pd3dCommandList, m_ppDepthRenderCameras[j]);

			::SynchronizeResourceTransition(pd3dCommandList, m_pDepthFromLightTexture->GetResource(j), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON);
		}
		else
		{
			m_pToLightSpaces->m_pxmf4LightPositions[j].w = 0.0f;
		}
	}
}

void CDepthRenderShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);


}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CShadowMapShader::CShadowMapShader()
{
}

CShadowMapShader::~CShadowMapShader()
{
}

D3D12_DEPTH_STENCIL_DESC CShadowMapShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CShadowMapShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSShadowMapShadow", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CShadowMapShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSShadowMapShadow", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void CShadowMapShader::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CShadowMapShader::ReleaseShaderVariables()
{
}

void CShadowMapShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pDepthFromLightTexture) m_pDepthFromLightTexture->UpdateShaderVariables(pd3dCommandList);
}

void CShadowMapShader::ReleaseUploadBuffers()
{
}

void CShadowMapShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
	// DepthRenderShader 에서 만들어준 texture을 인자로 받아서 연결
	m_pDepthFromLightTexture = (CTexture*)pContext;
	m_pDepthFromLightTexture->AddRef();

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CShadowMapShader::ReleaseObjects()
{
	if (m_pDepthFromLightTexture) m_pDepthFromLightTexture->Release();
}

void CShadowMapShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);

	UpdateShaderVariables(pd3dCommandList);

}
