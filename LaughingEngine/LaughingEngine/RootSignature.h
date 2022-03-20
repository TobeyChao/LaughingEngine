#pragma once
#include "PCH.h"

class RootParameter
{
public:
	void InitAsDescriptorTable(
		UINT numDescriptors,
		D3D12_DESCRIPTOR_RANGE_TYPE type,
		UINT baseRegister,
		UINT registerSpace = 0,
		D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
	{
		CD3DX12_DESCRIPTOR_RANGE* table = new CD3DX12_DESCRIPTOR_RANGE(type, numDescriptors, baseRegister, registerSpace);
		m_Param.InitAsDescriptorTable(1, table, visibility);
	}

	void InitAsConstants(
		UINT num32BitValues,
		UINT shaderRegister,
		UINT registerSpace = 0,
		D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
	{
		m_Param.InitAsConstants(
			num32BitValues,
			shaderRegister,
			registerSpace,
			visibility);
	}

	void InitAsConstantBufferView(
		UINT shaderRegister,
		UINT registerSpace = 0,
		D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
	{
		m_Param.InitAsConstantBufferView(
			shaderRegister,
			registerSpace,
			visibility);
	}

	void InitAsShaderResourceView(
		UINT shaderRegister,
		UINT registerSpace = 0,
		D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
	{
		m_Param.InitAsShaderResourceView(
			shaderRegister,
			registerSpace,
			visibility);
	}

	void InitAsUnorderedAccessView(
		UINT shaderRegister,
		UINT registerSpace = 0,
		D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
	{
		m_Param.InitAsUnorderedAccessView(
			shaderRegister,
			registerSpace,
			visibility);
	}

	const CD3DX12_ROOT_PARAMETER& operator() () const
	{
		return m_Param;
	}

private:
	CD3DX12_ROOT_PARAMETER m_Param;
};

class RootSignature
{
public:

	RootSignature() = default;
	~RootSignature() = default;

	void Destroy()
	{
		m_RootSignature.Reset();
	}

	void Reset(UINT NumRootParams, UINT NumStaticSamplers = 0)
	{
		if (NumRootParams > 0)
		{
			m_ParamArray.reset(new RootParameter[NumRootParams]);
		}
		else
		{
			m_ParamArray.reset();
		}
		m_NumParameters = NumRootParams;

		if (NumStaticSamplers > 0)
		{
			m_StaticSamplerArray.reset(new CD3DX12_STATIC_SAMPLER_DESC[NumStaticSamplers]);
		}
		else
		{
			m_StaticSamplerArray.reset();
		}
		m_NumStaticSamplers = NumStaticSamplers;
	}

	RootParameter& operator[] (size_t EntryIndex)
	{
		return m_ParamArray.get()[EntryIndex];
	}

	void InitStaticSampler(UINT Register, const D3D12_SAMPLER_DESC& NonStaticSamplerDesc,
		D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL);

	void Finalize(const std::wstring& name, D3D12_ROOT_SIGNATURE_FLAGS Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE);

	ID3D12RootSignature* GetRootSignature() const
	{
		return m_RootSignature.Get();
	}

private:
	BOOL m_Finalized;
	UINT m_NumParameters;
	UINT m_NumStaticSamplers;
	std::unique_ptr<RootParameter[]> m_ParamArray;
	std::unique_ptr<CD3DX12_STATIC_SAMPLER_DESC[]> m_StaticSamplerArray;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
};