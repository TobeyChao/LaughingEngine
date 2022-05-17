#include "PCH.h"
#include "RootSignature.h"
#include "GraphicsCore.h"

using namespace Microsoft::WRL;

RootSignature::RootSignature() :
	m_NumParameters(0),
	m_NumStaticSamplers(0),
	m_NumInitializedStaticSamplers(0)
{}

RootSignature::~RootSignature() = default;

void RootSignature::Destroy()
{
	m_RootSignature.Reset();
}

void RootSignature::Reset(UINT NumRootParams, UINT NumStaticSamplers)
{
	if (NumRootParams > 0)
	{
		m_ParamArray.resize(NumRootParams);
	}
	else
	{
		m_ParamArray.clear();
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

RootParameter& RootSignature::operator[](size_t EntryIndex)
{
	return m_ParamArray[EntryIndex];
}

ID3D12RootSignature* RootSignature::GetRootSignature() const
{
	return m_RootSignature.Get();
}

void RootSignature::InitStaticSampler(const D3D12_STATIC_SAMPLER_DESC & StaticSamplerDesc)
{
	assert(m_NumInitializedStaticSamplers < m_NumStaticSamplers);
	m_StaticSamplerArray[m_NumInitializedStaticSamplers] = StaticSamplerDesc;
	m_NumInitializedStaticSamplers++;
}

void RootSignature::Finalize(const std::wstring & name, D3D12_ROOT_SIGNATURE_FLAGS Flags)
{
	CD3DX12_ROOT_SIGNATURE_DESC rootSignDesc(
		m_NumParameters,
		(const CD3DX12_ROOT_PARAMETER*)m_ParamArray.data(),
		m_NumStaticSamplers,
		m_StaticSamplerArray.data(),
		Flags);

	ComPtr<ID3DBlob> serializedRootSign = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(
		&rootSignDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSign.GetAddressOf(),
		errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		Utility::Print((char*)errorBlob->GetBufferPointer());
	}

	ThrowIfFailed(hr);

	CreateRootSignature(name, serializedRootSign->GetBufferPointer(), serializedRootSign->GetBufferSize());
}

void RootSignature::CreateFromMemory(const std::wstring & name, const void* Data, size_t Size)
{
	ComPtr<ID3D12VersionedRootSignatureDeserializer> deserializer;

	ThrowIfFailed(D3D12CreateVersionedRootSignatureDeserializer(Data, Size, IID_PPV_ARGS(deserializer.GetAddressOf())));

	const D3D12_VERSIONED_ROOT_SIGNATURE_DESC* desc = deserializer->GetUnconvertedRootSignatureDesc();

	Reset(desc->Desc_1_1.NumParameters, desc->Desc_1_1.NumStaticSamplers);

	for (UINT i = 0; i < m_NumParameters; ++i)
	{
		switch (desc->Desc_1_1.pParameters[i].ParameterType)
		{
		case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
		{
			auto& DescriptorTable = desc->Desc_1_1.pParameters[i].DescriptorTable;
			const UINT NumDescriptorRanges = DescriptorTable.NumDescriptorRanges;
			operator[](i).InitAsDescriptorTable(NumDescriptorRanges);
			for (UINT j = 0; j < NumDescriptorRanges; ++j)
			{
				auto& range = DescriptorTable.pDescriptorRanges[j];
				operator[](i).SetTableRange(j, range.RangeType, range.NumDescriptors, range.BaseShaderRegister, range.RegisterSpace);
			}
		}

		break;
		case D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS:
		{
			auto& Constants = desc->Desc_1_1.pParameters[i].Constants;
			operator[](i).InitAsConstants(Constants.Num32BitValues, Constants.ShaderRegister, Constants.RegisterSpace);
		}

		break;
		case D3D12_ROOT_PARAMETER_TYPE_CBV:
		{
			auto& CBVDescriptor = desc->Desc_1_1.pParameters[i].Descriptor;
			operator[](i).InitAsConstantBufferView(CBVDescriptor.ShaderRegister, CBVDescriptor.RegisterSpace);
		}
		break;
		case D3D12_ROOT_PARAMETER_TYPE_SRV:
		{
			auto& SRVDescriptor = desc->Desc_1_1.pParameters[i].Descriptor;
			operator[](i).InitAsConstantBufferView(SRVDescriptor.ShaderRegister, SRVDescriptor.RegisterSpace);
		}
		break;
		case D3D12_ROOT_PARAMETER_TYPE_UAV:
		{
			auto& UAVDescriptor = desc->Desc_1_1.pParameters[i].Descriptor;
			operator[](i).InitAsConstantBufferView(UAVDescriptor.ShaderRegister, UAVDescriptor.RegisterSpace);
		}
		break;
		}
	}

	for (UINT i = 0; i < m_NumStaticSamplers; ++i)
	{
		InitStaticSampler(desc->Desc_1_1.pStaticSamplers[i]);
	}

	CreateRootSignature(name, Data, Size);
}

void RootSignature::CreateRootSignature(const std::wstring & name, const void* Data, size_t Size)
{
	m_DescriptorTableBitMap = 0;
	m_SamplerTableBitMap = 0;

	for (UINT Param = 0; Param < m_NumParameters; ++Param)
	{
		const D3D12_ROOT_PARAMETER& RootParam = m_ParamArray[Param].m_Param;
		m_DescriptorTableSize[Param] = 0;

		if (RootParam.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
		{
			assert(RootParam.DescriptorTable.pDescriptorRanges != nullptr);

			// We keep track of sampler descriptor tables separately from CBV_SRV_UAV descriptor tables
			if (RootParam.DescriptorTable.pDescriptorRanges->RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER)
				m_SamplerTableBitMap |= (1 << Param);
			else
				m_DescriptorTableBitMap |= (1 << Param);

			for (UINT TableRange = 0; TableRange < RootParam.DescriptorTable.NumDescriptorRanges; ++TableRange)
			{
				m_DescriptorTableSize[Param] += RootParam.DescriptorTable.pDescriptorRanges[TableRange].NumDescriptors;
			}
		}
	}

	ThrowIfFailed(Graphics::g_Device->CreateRootSignature(0, Data, Size, IID_PPV_ARGS(m_RootSignature.GetAddressOf())));

#if defined(_DEBUG) || defined(DEBUG)
	m_RootSignature->SetName(name.c_str());
#endif // defined(_DEBUG) || defined(DEBUG)
}
