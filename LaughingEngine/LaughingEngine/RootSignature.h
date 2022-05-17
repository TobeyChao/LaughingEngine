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
	friend class RootSignature;
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

	void SetTableRange(UINT RangeIndex, D3D12_DESCRIPTOR_RANGE_TYPE Type, UINT Count, UINT Register, UINT Space = 0)
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
	CD3DX12_ROOT_PARAMETER m_Param{};
};

class RootSignature
{
	friend class DynamicDescriptorHeap;
public:
	RootSignature();

	~RootSignature();

	void Destroy();

	void Reset(UINT NumRootParams, UINT NumStaticSamplers = 0);

	RootParameter& operator[] (size_t EntryIndex);

	void InitStaticSampler(const D3D12_STATIC_SAMPLER_DESC& StaticSamplerDesc);

	void Finalize(const std::wstring& Name, D3D12_ROOT_SIGNATURE_FLAGS Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE);

	void CreateFromMemory(const std::wstring& Name, const void* Data, size_t Size);

	[[nodiscard]] ID3D12RootSignature* GetRootSignature() const;

private:
	void CreateRootSignature(const std::wstring& name, const void* Data, size_t Size);

private:
	UINT m_NumParameters;
	UINT m_NumStaticSamplers;
	UINT m_NumInitializedStaticSamplers;

	uint32_t m_DescriptorTableBitMap;		// One bit is set for root parameters that are non-sampler descriptor tables
	uint32_t m_SamplerTableBitMap;			// One bit is set for root parameters that are sampler descriptor tables
	uint32_t m_DescriptorTableSize[16];		// Non-sampler descriptor tables need to know their descriptor count

	std::vector<RootParameter> m_ParamArray;
	std::vector<D3D12_STATIC_SAMPLER_DESC> m_StaticSamplerArray;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
};