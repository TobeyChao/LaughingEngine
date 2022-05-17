#pragma once
#include "PCH.h"
#include "BufferManager.h"

class RenderTargetStorage : public TSingleton<RenderTargetStorage>
{
public:
	void Load()
	{
		MainViewport.Width = (float)Graphics::g_SceneColorBuffer.GetWidth();
		MainViewport.Height = (float)Graphics::g_SceneColorBuffer.GetHeight();
		MainViewport.MinDepth = 0.0f;
		MainViewport.MaxDepth = 1.0f;

		MainScissor.left = 0;
		MainScissor.top = 0;
		MainScissor.right = (LONG)Graphics::g_SceneColorBuffer.GetWidth();
		MainScissor.bottom = (LONG)Graphics::g_SceneColorBuffer.GetHeight();
	}

	void Shutdown()
	{

	}

	CD3DX12_VIEWPORT MainViewport;
	CD3DX12_RECT MainScissor;
};
