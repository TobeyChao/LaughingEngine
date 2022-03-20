#include "GraphicsCommon.h"

namespace Graphics
{
	D3D12_RASTERIZER_DESC RasterizerDefault;
	D3D12_RASTERIZER_DESC RasterizerTwoSided;
}

void Graphics::InitializeCommonState()
{
	RasterizerDefault = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	RasterizerTwoSided = RasterizerDefault;
	RasterizerTwoSided.CullMode = D3D12_CULL_MODE_NONE;
}

void Graphics::DestroyCommonState()
{

}