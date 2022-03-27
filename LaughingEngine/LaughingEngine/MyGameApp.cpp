#include "MyGameApp.h"
#include "TextureManager.h"
#include "BufferManager.h"
#include "CommandContext.h"
#include "CommandListManager.h"

using namespace Graphics;

void MyGameApp::Initialize()
{
	TextureRef ref = TextureManager::LoadDDSFromFile(L"../Assets/Textures/WoodCrate02.dds");
	Utility::Printf("texture: %p", ref.Get());
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
	const D3D12_VIEWPORT& viewport = m_MainViewport;
	const D3D12_RECT& scissor = m_MainScissor;
	GraphicsContext& context = GraphicsContext::Begin(L"Scene Render");
	context.TransitionResource(g_SceneColorBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, true);
	context.SetRenderTarget(g_SceneColorBuffer.GetRTV());
	context.SetViewportAndScissorRect(viewport, scissor);
	context.ClearColor(g_SceneColorBuffer, &scissor);
	context.Finish();
}

void MyGameApp::Shutdown()
{
}