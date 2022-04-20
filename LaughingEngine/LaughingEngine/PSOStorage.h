#pragma once
#include <CompiledShaders/CommonRS.h>
#include <CompiledShaders/LightPS.h>
#include <CompiledShaders/LightVS.h>
#include <CompiledShaders/SkyPS.h>
#include <CompiledShaders/SkyVS.h>

#include "BufferManager.h"
#include "GraphicsCommon.h"
#include "PipelineState.h"
#include "RootSignature.h"

class PSOStorage : public TSingleton<PSOStorage>
{
public:
	PSOStorage()
		:
		DefaultPSO(L"MyGameApp::DefaultPSO"),
		SkyPSO(L"MyGameApp::SkyPSO")
	{}

	void Load()
	{
		D3D12_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		m_DefaultRS.CreateFromMemory(L"MyGameApp::m_DefaultRS", g_pCommonRS, sizeof(g_pCommonRS));

		DefaultPSO.SetRootSignature(m_DefaultRS);
		DefaultPSO.SetRasterizerState(Graphics::RasterizerDefault);
		DefaultPSO.SetBlendState(CD3DX12_BLEND_DESC(D3D12_DEFAULT));
		DefaultPSO.SetDepthStencilState(CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT));
		DefaultPSO.SetSampleMask(UINT_MAX);
		DefaultPSO.SetInputLayout(_countof(inputElementDesc), inputElementDesc);
		DefaultPSO.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		DefaultPSO.SetVertexShader(g_pLightVS, sizeof(g_pLightVS));
		DefaultPSO.SetPixelShader(g_pLightPS, sizeof(g_pLightPS));
		DefaultPSO.SetRenderTargetFormat(Graphics::g_DefaultHdrColorFormat, Graphics::g_DefaultDepthStencilFormat);
		DefaultPSO.Finalize();

		SkyPSO = DefaultPSO;
		SkyPSO.SetDepthStencilState(Graphics::DepthStateTestLessEqual);
		SkyPSO.SetRasterizerState(Graphics::RasterizerTwoSided);
		SkyPSO.SetVertexShader(g_pSkyVS, sizeof(g_pSkyVS));
		SkyPSO.SetPixelShader(g_pSkyPS, sizeof(g_pSkyPS));
		SkyPSO.SetInputLayout(0, nullptr);
		SkyPSO.Finalize();
	}

	RootSignature m_DefaultRS;

	GraphicsPiplelineState DefaultPSO;
	GraphicsPiplelineState SkyPSO;
};
