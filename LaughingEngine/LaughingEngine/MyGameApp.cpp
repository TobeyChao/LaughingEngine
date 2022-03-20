#include "MyGameApp.h"

#include "BufferManager.h"
#include "CommandContext.h"

using namespace Graphics;

void MyGameApp::Initialize()
{
}

void MyGameApp::Update()
{
	m_MainViewport.Width = (float)g_SceneColorBuffer.GetWidth();
	m_MainViewport.Height = (float)g_SceneColorBuffer.GetHeight();
	m_MainViewport.MinDepth = 0.0f;
	m_MainViewport.MaxDepth = 1.0f;

	m_MainScissor.left = 0;
	m_MainScissor.top = 0;
	m_MainScissor.right = (LONG)g_SceneColorBuffer.GetWidth();
	m_MainScissor.bottom = (LONG)g_SceneColorBuffer.GetHeight();
}

void MyGameApp::Draw()
{
	GraphicsContext& context = GraphicsContext::Begin(L"Scene Render");
	const D3D12_VIEWPORT& viewport = m_MainViewport;
	const D3D12_RECT& scissor = m_MainScissor;

	context.TransitionResource(g_SceneColorBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, true);
	context.SetRenderTarget(g_SceneColorBuffer.GetRTV());
	context.SetViewportAndScissorRect(viewport, scissor);
	context.ClearColor(g_SceneColorBuffer, &scissor);

	context.Finish();
}

void MyGameApp::Shutdown()
{
}