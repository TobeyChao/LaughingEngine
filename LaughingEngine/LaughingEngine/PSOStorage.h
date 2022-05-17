#pragma once
#include "BufferManager.h"
#include "GraphicsCommon.h"
#include "PipelineState.h"
#include "RootSignature.h"
#include "ShaderManager.h"

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

		auto rs = Utility::LoadFileSync(L"Assets\\CompiledShaders\\CommonRS.cso");
		DefaultRS.CreateFromMemory(L"MyGameApp::m_DefaultRS", rs->data(), rs->size());

		DefaultPSO.SetRootSignature(DefaultRS);
		DefaultPSO.SetRasterizerState(Graphics::RasterizerDefault);
		DefaultPSO.SetBlendState(CD3DX12_BLEND_DESC(D3D12_DEFAULT));
		DefaultPSO.SetDepthStencilState(CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT));
		DefaultPSO.SetSampleMask(UINT_MAX);
		DefaultPSO.SetInputLayout(_countof(inputElementDesc), inputElementDesc);
		DefaultPSO.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		auto light = ShaderManager::GetInstance().GetShader(L"Light");
		DefaultPSO.SetVertexShader(light->GetShader(ShaderType::Vertex)->data(), light->GetShader(ShaderType::Vertex)->size());
		DefaultPSO.SetPixelShader(light->GetShader(ShaderType::Pixel)->data(), light->GetShader(ShaderType::Pixel)->size());
		DefaultPSO.SetRenderTargetFormat(Graphics::g_DefaultHdrColorFormat, Graphics::g_DefaultDepthStencilFormat);
		DefaultPSO.Finalize();

		SkyPSO = DefaultPSO;
		SkyPSO.SetDepthStencilState(Graphics::DepthStateTestLessEqual);
		SkyPSO.SetRasterizerState(Graphics::RasterizerTwoSided);
		auto sky = ShaderManager::GetInstance().GetShader(L"Sky");
		SkyPSO.SetVertexShader(sky->GetShader(ShaderType::Vertex)->data(), sky->GetShader(ShaderType::Vertex)->size());
		SkyPSO.SetPixelShader(sky->GetShader(ShaderType::Pixel)->data(), sky->GetShader(ShaderType::Pixel)->size());
		SkyPSO.SetInputLayout(0, nullptr);
		SkyPSO.Finalize();
	}

	RootSignature DefaultRS;

	GraphicsPiplelineState DefaultPSO;
	GraphicsPiplelineState SkyPSO;
};
