#include "RootSignature.h"
#include "GraphicsCore.h"

using namespace Microsoft::WRL;

void RootSignature::InitStaticSampler(UINT Register, const D3D12_SAMPLER_DESC& NonStaticSamplerDesc, D3D12_SHADER_VISIBILITY Visibility)
{

}

void RootSignature::Finalize(const std::wstring& name, D3D12_ROOT_SIGNATURE_FLAGS Flags)
{
	CD3DX12_ROOT_SIGNATURE_DESC rootSignDesc(
		m_NumParameters,
		(const CD3DX12_ROOT_PARAMETER*)m_ParamArray.get(),
		m_NumStaticSamplers,
		m_StaticSamplerArray.get(),
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

	CreateFromMemory(name, serializedRootSign->GetBufferPointer(), serializedRootSign->GetBufferSize());
}

void RootSignature::CreateFromMemory(const std::wstring& name, const void* Data, size_t Size)
{
	ThrowIfFailed(Graphics::g_Device->CreateRootSignature(0, Data, Size, IID_PPV_ARGS(m_RootSignature.GetAddressOf())));

#if defined(_DEBUG) || defined(DEBUG)
	m_RootSignature->SetName(name.c_str());
#endif // defined(_DEBUG) || defined(DEBUG)
}