#pragma once
#include "DepthBuffer.h"

class ShadowMapBuffer : public DepthBuffer
{
public:
	void Create(const std::wstring& Name, uint32_t Width, uint32_t Height);

	void BeginRendering(GraphicsContext& Context);
	void EndRendering(GraphicsContext& Context);

private:
	D3D12_VIEWPORT m_Viewport;
	D3D12_RECT m_Scissor;
};