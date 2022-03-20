#pragma once

#include "PCH.h"

namespace Graphics
{
	void InitializeCommonState();
	void DestroyCommonState();

	extern D3D12_RASTERIZER_DESC RasterizerDefault;
	extern D3D12_RASTERIZER_DESC RasterizerTwoSided;
}