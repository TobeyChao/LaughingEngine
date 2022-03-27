#include "Texture.h"
#include "GraphicsCore.h"

#include "DDSTextureLoader12.h"

void Texture::Create2D()
{
}

bool Texture::CreateDDSFromMemory(const void* Data, size_t Size, bool IssRGB)
{
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	HRESULT hr = DirectX::LoadDDSTextureFromMemory(
		Graphics::g_Device,
		(const uint8_t*)Data,
		Size,
		m_pResource.GetAddressOf(),
		subresources,
		0,
		nullptr,
		nullptr
	);

	if (FAILED(hr))
	{
		return false;
	}

	if (m_hCpuDescriptorHandle.ptr == -1)
	{
		m_hCpuDescriptorHandle = Graphics::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	D3D12_RESOURCE_DESC desc = m_pResource->GetDesc();
	m_Width = (uint32_t)desc.Width;
	m_Height = desc.Height;
	m_Depth = desc.DepthOrArraySize;

	Graphics::g_Device->CreateShaderResourceView(m_pResource.Get(), nullptr, m_hCpuDescriptorHandle);

	return true;
}
