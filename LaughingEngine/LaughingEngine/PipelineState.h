#pragma once
#include "PCH.h"

class CommandContext;
class RootSignature;

class PipelineState
{
public:
	explicit PipelineState(const std::wstring& Name)
		:
		m_Name(Name),
		m_RootSignature(nullptr),
		m_PSO(nullptr)
	{}

	void Destroy()
	{
		m_PSO.Reset();
	}

	void SetRootSignature(const RootSignature& rootSignature)
	{
		m_RootSignature = &rootSignature;
	}

	ID3D12PipelineState* GetPSO() const
	{
		return m_PSO.Get();
	}

protected:
	std::wstring m_Name;
	const RootSignature* m_RootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PSO;
};

class GraphicsPiplelineState : public PipelineState
{
public:
	GraphicsPiplelineState(const std::wstring& Name);

	void SetBlendState(const D3D12_BLEND_DESC& BlendDesc);
	void SetRasterizerState(const D3D12_RASTERIZER_DESC& RasterizerDesc);
	void SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& DepthStencilDesc);
	void SetSampleMask(UINT SampleMask);
	void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE TopologyType);
	void SetDepthTargetFormat(DXGI_FORMAT DSVFormat, UINT MsaaCount = 1, UINT MsaaQuality = 0);
	void SetRenderTargetFormat(DXGI_FORMAT RTVFormat, DXGI_FORMAT DSVFormat, UINT MsaaCount = 1, UINT MsaaQuality = 0);
	void SetRenderTargetFormats(UINT NumRTVs, const DXGI_FORMAT* RTVFormats, DXGI_FORMAT DSVFormat, UINT MsaaCount = 1, UINT MsaaQuality = 0);
	void SetInputLayout(UINT NumElements, const D3D12_INPUT_ELEMENT_DESC* pInputElementDescs);

	void SetVertexShader(const void* Binary, size_t Size) { m_PSODesc.VS = CD3DX12_SHADER_BYTECODE(const_cast<void*>(Binary), Size); }
	void SetPixelShader(const void* Binary, size_t Size) { m_PSODesc.PS = CD3DX12_SHADER_BYTECODE(const_cast<void*>(Binary), Size); }
	void SetGeometryShader(const void* Binary, size_t Size) { m_PSODesc.GS = CD3DX12_SHADER_BYTECODE(const_cast<void*>(Binary), Size); }
	void SetHullShader(const void* Binary, size_t Size) { m_PSODesc.HS = CD3DX12_SHADER_BYTECODE(const_cast<void*>(Binary), Size); }
	void SetDomainShader(const void* Binary, size_t Size) { m_PSODesc.DS = CD3DX12_SHADER_BYTECODE(const_cast<void*>(Binary), Size); }

	void SetVertexShader(const D3D12_SHADER_BYTECODE& Binary) { m_PSODesc.VS = Binary; }
	void SetPixelShader(const D3D12_SHADER_BYTECODE& Binary) { m_PSODesc.PS = Binary; }
	void SetGeometryShader(const D3D12_SHADER_BYTECODE& Binary) { m_PSODesc.GS = Binary; }
	void SetHullShader(const D3D12_SHADER_BYTECODE& Binary) { m_PSODesc.HS = Binary; }
	void SetDomainShader(const D3D12_SHADER_BYTECODE& Binary) { m_PSODesc.DS = Binary; }

	void Finalize();

private:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC m_PSODesc;
	std::shared_ptr<const D3D12_INPUT_ELEMENT_DESC> m_InputLayouts;
};

class ComputePiplelineState : public PipelineState
{

};