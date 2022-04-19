#pragma once
#include "PCH.h"

// https://docs.microsoft.com/en-us/windows/win32/direct3d12/root-signatures

/// <summary>
/// RootParameter:
/// root constants (constants inlined in the root arguments), cost 1 DWORD each, since they are 32-bit values.
/// root descriptors (descriptors inlined in the root arguments), (64-bit GPU virtual addresses) cost 2 DWORDs each.
/// descriptor tables (pointers to a range of descriptors in the descriptor heap), cost 1 DWORD each.
/// </summary>
class RootParameter
{
public:
	void InitAsDescriptorTable(UINT NumDescriptors, D3D12_DESCRIPTOR_RANGE_TYPE Type, UINT BaseRegister, UINT RegisterSpace = 0, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL)
	{
		CD3DX12_DESCRIPTOR_RANGE* table = new CD3DX12_DESCRIPTOR_RANGE(Type, NumDescriptors, BaseRegister, RegisterSpace);
		m_Param.InitAsDescriptorTable(1, table, Visibility);
	}

	void InitAsDescriptorTable(UINT RangeCount, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL)
	{
		CD3DX12_DESCRIPTOR_RANGE* table = new CD3DX12_DESCRIPTOR_RANGE[RangeCount];
		m_Param.InitAsDescriptorTable(RangeCount, table, Visibility);
	}

	void SetTableRange(UINT RangeIndex, D3D12_DESCRIPTOR_RANGE_TYPE Type, UINT Register, UINT Count, UINT Space = 0)
	{
		D3D12_DESCRIPTOR_RANGE& range = const_cast<D3D12_DESCRIPTOR_RANGE&>(m_Param.DescriptorTable.pDescriptorRanges[RangeIndex]);
		range.RangeType = Type;
		range.NumDescriptors = Count;
		range.BaseShaderRegister = Register;
		range.RegisterSpace = Space;
		range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	}

	void InitAsConstants(UINT Num32BitValues, UINT ShaderRegister, UINT RegisterSpace = 0, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL)
	{
		m_Param.InitAsConstants(Num32BitValues, ShaderRegister, RegisterSpace, Visibility);
	}

	void InitAsConstantBufferView(UINT ShaderRegister, UINT RegisterSpace = 0, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL)
	{
		m_Param.InitAsConstantBufferView(ShaderRegister, RegisterSpace, Visibility);
	}

	void InitAsShaderResourceView(UINT ShaderRegister, UINT RegisterSpace = 0, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL)
	{
		m_Param.InitAsShaderResourceView(ShaderRegister, RegisterSpace, Visibility);
	}

	void InitAsUnorderedAccessView(UINT ShaderRegister, UINT RegisterSpace = 0, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL)
	{
		m_Param.InitAsUnorderedAccessView(ShaderRegister, RegisterSpace, Visibility);
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

	RootSignature()
		:
		m_Finalized(FALSE),
		m_NumParameters(0),
		m_NumStaticSamplers(0),
		m_NumInitializedStaticSamplers(0)
	{};

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
			m_StaticSamplerArray.resize(NumStaticSamplers);
		}
		else
		{
			m_StaticSamplerArray.clear();
		}
		m_NumStaticSamplers = NumStaticSamplers;
		m_NumInitializedStaticSamplers = 0;
	}

	RootParameter& operator[] (size_t EntryIndex)
	{
		return m_ParamArray.get()[EntryIndex];
	}

	void InitStaticSampler(CD3DX12_STATIC_SAMPLER_DESC StaticSamplerDesc, UINT ShaderRegister, UINT RegisterSpace = 0);

	void Finalize(const std::wstring& Name, D3D12_ROOT_SIGNATURE_FLAGS Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE);

	void CreateFromMemory(const std::wstring& Name, const void* Data, size_t Size);

	ID3D12RootSignature* GetRootSignature() const
	{
		return m_RootSignature.Get();
	}

private:
	BOOL m_Finalized;
	UINT m_NumParameters;
	UINT m_NumStaticSamplers;
	UINT m_NumInitializedStaticSamplers;
	std::unique_ptr<RootParameter[]> m_ParamArray;
	std::vector<CD3DX12_STATIC_SAMPLER_DESC> m_StaticSamplerArray;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
};