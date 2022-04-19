#include "PCH.h"
#include "BufferManager.h"
#include "CommandContext.h"

namespace Graphics
{
	ColorBuffer g_SceneColorBuffer;
	DepthBuffer g_SceneDepthBuffer;

	DXGI_FORMAT g_DefaultHdrColorFormat = DXGI_FORMAT_R11G11B10_FLOAT;
	DXGI_FORMAT g_DefaultDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	void InitializeRenderingBuffers(uint32_t Width, uint32_t Height)
	{
		GraphicsContext& Context = GraphicsContext::Begin();
		g_SceneColorBuffer.Create(L"SceneColorBuffer", Width, Height, 1, g_DefaultHdrColorFormat);
		g_SceneDepthBuffer.Create(L"SceneDepthBuffer", Width, Height, g_DefaultDepthStencilFormat);
		Context.TransitionResource(g_SceneDepthBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		Context.Finish();
	}

	void ResizeDisplayDependentBuffers(uint32_t Width, uint32_t Height)
	{
	}

	void DestroyRenderingBuffers()
	{
		g_SceneColorBuffer.Destroy();
		g_SceneDepthBuffer.Destroy();
	}
}