#include "BufferManager.h"
#include "CommandContext.h"

namespace Graphics
{
	ColorBuffer g_SceneColorBuffer;

	DXGI_FORMAT g_DefaultHdrColorFormat = DXGI_FORMAT_R11G11B10_FLOAT;

	void InitializeRenderingBuffers(uint32_t Width, uint32_t Height)
	{
		GraphicsContext& Context = GraphicsContext::Begin();

		g_SceneColorBuffer.Create(L"SceneColorBuffer", Width, Height, 1, g_DefaultHdrColorFormat);

		Context.Finish();
	}

	void ResizeDisplayDependentBuffers(uint32_t Width, uint32_t Height)
	{
	}

	void DestroyRenderingBuffers()
	{
		g_SceneColorBuffer.Destroy();
	}
}