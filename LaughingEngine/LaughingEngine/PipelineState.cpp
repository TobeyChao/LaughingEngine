#include "PipelineState.h"
#include "RootSignature.h"
#include "GraphicsCore.h"

using namespace Graphics;

GraphicsPiplelineState::GraphicsPiplelineState(const std::wstring& Name)
	:
	PipelineState(Name)
{
	ZeroMemory(&m_PSODesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
}

void GraphicsPiplelineState::SetBlendState(const D3D12_BLEND_DESC& BlendDesc)
{
	m_PSODesc.BlendState = BlendDesc;
}

void GraphicsPiplelineState::SetRasterizerState(const D3D12_RASTERIZER_DESC& RasterizerDesc)
{
	m_PSODesc.RasterizerState = RasterizerDesc;
}

void GraphicsPiplelineState::SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& DepthStencilDesc)
{
	m_PSODesc.DepthStencilState = DepthStencilDesc;
}

void GraphicsPiplelineState::SetSampleMask(UINT SampleMask)
{
	m_PSODesc.SampleMask = SampleMask;
}

void GraphicsPiplelineState::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE TopologyType)
{
	m_PSODesc.PrimitiveTopologyType = TopologyType;
}

void GraphicsPiplelineState::SetDepthTargetFormat(DXGI_FORMAT DSVFormat, UINT MsaaCount, UINT MsaaQuality)
{
	SetRenderTargetFormats(0, nullptr, DSVFormat, MsaaCount, MsaaQuality);
}

void GraphicsPiplelineState::SetRenderTargetFormat(DXGI_FORMAT RTVFormat, DXGI_FORMAT DSVFormat, UINT MsaaCount, UINT MsaaQuality)
{
	SetRenderTargetFormats(1, &RTVFormat, DSVFormat, MsaaCount, MsaaQuality);
}

void GraphicsPiplelineState::SetRenderTargetFormats(UINT NumRTVs, const DXGI_FORMAT* RTVFormats, DXGI_FORMAT DSVFormat, UINT MsaaCount, UINT MsaaQuality)
{
	for (UINT i = 0; i < NumRTVs; i++)
	{
		m_PSODesc.RTVFormats[i] = RTVFormats[i];
	}
	for (UINT i = NumRTVs; i < m_PSODesc.NumRenderTargets; i++)
	{
		m_PSODesc.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;
	}
	m_PSODesc.NumRenderTargets = NumRTVs;
	m_PSODesc.DSVFormat = DSVFormat;
	m_PSODesc.SampleDesc.Count = MsaaCount;
	m_PSODesc.SampleDesc.Quality = MsaaQuality;
}

void GraphicsPiplelineState::SetInputLayout(UINT NumElements, const D3D12_INPUT_ELEMENT_DESC* pInputElementDescs)
{
	if (NumElements > 0)
	{
		D3D12_INPUT_ELEMENT_DESC* desc = new D3D12_INPUT_ELEMENT_DESC[NumElements];
		memcpy(desc, pInputElementDescs, NumElements * sizeof(D3D12_INPUT_ELEMENT_DESC));
		m_InputLayouts.reset(desc);
	}
	else
	{
		m_InputLayouts.reset();
	}

	m_PSODesc.InputLayout.NumElements = NumElements;
	m_PSODesc.InputLayout.pInputElementDescs = m_InputLayouts.get();
}

void GraphicsPiplelineState::Finalize()
{
	assert(m_RootSignature != nullptr);
	m_PSODesc.pRootSignature = m_RootSignature->GetRootSignature();
	ThrowIfFailed(g_Device->CreateGraphicsPipelineState(&m_PSODesc, IID_PPV_ARGS(&m_PSO)));

#if defined(_DEBUG) || defined(DEBUG)
	m_PSO->SetName(m_Name.c_str());
#endif // defined(_DEBUG) || defined(DEBUG)
}