#include "MyGameApp.h"
#include "TextureManager.h"
#include "BufferManager.h"
#include "CommandContext.h"
#include "CommandListManager.h"
#include "GraphicsCommon.h"
#include "UploadBuffer.h"
#include "Display.h"
#include "GameTimer.h"
#include <array>

// Shader
#include "CompiledShaders/ColorVS.h"
#include "CompiledShaders/ColorPS.h"
#include "CompiledShaders/ColorRS.h"

#include "CompiledShaders/LightVS.h"
#include "CompiledShaders/LightPS.h"
#include "CompiledShaders/LightRS.h"

using namespace Graphics;

void MyGameApp::Initialize()
{
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	//m_DefaultRS.Reset(1);
	//m_DefaultRS[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
	//m_DefaultRS.Finalize(L"MyGameApp::m_DefaultRS", D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	m_DefaultRS.CreateFromMemory(L"MyGameApp::m_DefaultRS", g_pLightRS, sizeof(g_pLightRS));

	m_DefaultPSO.SetRootSignature(m_DefaultRS);
	m_DefaultPSO.SetRasterizerState(RasterizerDefault);
	m_DefaultPSO.SetBlendState(CD3DX12_BLEND_DESC(D3D12_DEFAULT));
	m_DefaultPSO.SetDepthStencilState(CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT));
	m_DefaultPSO.SetSampleMask(UINT_MAX);
	m_DefaultPSO.SetInputLayout(2, inputElementDesc);
	m_DefaultPSO.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	m_DefaultPSO.SetVertexShader(g_pLightVS, sizeof(g_pLightVS));
	m_DefaultPSO.SetPixelShader(g_pLightPS, sizeof(g_pLightPS));
	m_DefaultPSO.SetRenderTargetFormat(g_DefaultHdrColorFormat, g_DefaultDepthStencilFormat);
	m_DefaultPSO.Finalize();

	std::array<Vertex, 8> vertices =
	{
		Vertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White) }),
		Vertex({ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Black) }),
		Vertex({ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red) }),
		Vertex({ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green) }),
		Vertex({ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Blue) }),
		Vertex({ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow) }),
		Vertex({ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan) }),
		Vertex({ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Magenta) })
	};

	std::array<std::uint16_t, 36> indices =
	{
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	UploadBuffer uploadBufferV;
	uploadBufferV.Create(L"Temp uploadBufferV", vbByteSize);
	memcpy(uploadBufferV.Map(), vertices.data(), vbByteSize);
	uploadBufferV.Unmap();

	UploadBuffer uploadBufferI;
	uploadBufferI.Create(L"Temp uploadBufferI", ibByteSize);
	memcpy(uploadBufferI.Map(), indices.data(), ibByteSize);
	uploadBufferI.Unmap();

	m_BoxVertexBuffer.Create(L"MyGameApp::m_BoxVertexBuffer", sizeof(Vertex), (UINT)vertices.size(), uploadBufferV, 0);
	m_BoxIndexBuffer.Create(L"MyGameApp::m_BoxIndexBuffer", sizeof(sizeof(std::uint16_t)), (UINT)indices.size(), uploadBufferI, 0);

	m_BoxIndexBufferView = m_BoxIndexBuffer.IndexBufferView(0);
	m_BoxVertexBufferView = m_BoxVertexBuffer.VertexBufferView(0);

	m_Cameras["MainCamera"] = std::make_unique<Camera>();
	m_Cameras["MainCamera"]->SetPosition3f({ 0, 3, -10 });
	m_Cameras["MainCamera"]->SetLens(XM_PIDIV4, (float)Graphics::g_DisplayWidth / Graphics::g_DisplayHeight, 0.1f, 100.0f);
	m_Cameras["MainCamera"]->Pitch(0.3f);
	m_Cameras["MainCamera"]->ComputeInfo();

	//auto world = XMMatrixRotationY(XM_PIDIV4) * XMMatrixIdentity();
	//auto& view = m_Cameras["MainCamera"]->GetViewMatrix();
	//auto& proj = m_Cameras["MainCamera"]->GetProjMatrix();
	//auto worldViewProj = world * view * proj;
	//ObjectConstants objConstants;
	//XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(worldViewProj));

	// Pass
	XMMATRIX proj = m_Cameras["MainCamera"]->GetProjMatrix();
	XMMATRIX view = m_Cameras["MainCamera"]->GetViewMatrix();

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMVECTOR viewDet = XMMatrixDeterminant(view);
	XMVECTOR projDet = XMMatrixDeterminant(proj);
	XMVECTOR viewProjDet = XMMatrixDeterminant(viewProj);
	XMMATRIX invView = XMMatrixInverse(&viewDet, view);
	XMMATRIX invProj = XMMatrixInverse(&projDet, proj);
	XMMATRIX invViewProj = XMMatrixInverse(&viewProjDet, viewProj);
	//XMMATRIX shadowTransform = XMLoadFloat4x4(&mShadowTransform);

	XMStoreFloat4x4(&m_MainPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&m_MainPassCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&m_MainPassCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&m_MainPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&m_MainPassCB.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&m_MainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
	//XMStoreFloat4x4(&mMainPassCB.ShadowTransform, XMMatrixTranspose(shadowTransform));

	m_MainPassCB.EyePosW = m_Cameras["MainCamera"]->GetPosition3f();
	m_MainPassCB.RenderTargetSize = XMFLOAT2{ (float)g_DisplayWidth, (float)g_DisplayHeight };
	m_MainPassCB.InvRenderTargetSize = { 1.0f / g_DisplayWidth, 1.0f / g_DisplayHeight };
	m_MainPassCB.NearZ = 0.1f;
	m_MainPassCB.FarZ = 1000.0f;
	m_MainPassCB.TotalTime = (float)GameTimer::TotalTime();
	m_MainPassCB.DeltaTime = (float)GameTimer::DeltaTime();
	m_MainPassCB.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };

	m_MainPassCB.Lights[0].Direction = { 0.0f, 0.0f, 1.0f };
	m_MainPassCB.Lights[0].Strength = { 1.0f, 1.0f, 1.0f };
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

	// Obj
	m_Angle += (float)GameTimer::DeltaTime();
	if (m_Angle > XM_2PI)
	{
		m_Angle = 0.0f;
	}
	auto world = XMMatrixRotationY(m_Angle) * XMMatrixIdentity();
	XMStoreFloat4x4(&m_ObjPerObject.World, XMMatrixTranspose(world));
}

void MyGameApp::Draw()
{
	const D3D12_VIEWPORT& Viewport = m_MainViewport;
	const D3D12_RECT& Scissor = m_MainScissor;
	GraphicsContext& Context = GraphicsContext::Begin(L"Scene Render");
	Context.SetRootSignature(m_DefaultRS);
	Context.SetPipelineState(m_DefaultPSO);
	Context.TransitionResource(g_SceneColorBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, true);
	Context.SetRenderTarget(g_SceneColorBuffer.GetRTV(), g_SceneDepthBuffer.GetDSV());
	Context.ClearColor(g_SceneColorBuffer, &Scissor);
	Context.ClearDepth(g_SceneDepthBuffer);
	Context.SetViewportAndScissorRect(Viewport, Scissor);
	Context.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//Context.SetConstantBuffer(0, m_ObjPerObject.GetGpuAddress());
	Context.SetDynamicConstantBufferView(0, sizeof(m_MainPassCB), &m_MainPassCB);
	Context.SetDynamicConstantBufferView(1, sizeof(m_ObjPerObject), &m_ObjPerObject);
	Context.SetIndexBuffer(m_BoxIndexBufferView);
	Context.SetVertexBuffer(0, m_BoxVertexBufferView);
	Context.DrawIndexedInstanced(36, 1, 0, 0, 0);
	Context.Finish();
}

void MyGameApp::Shutdown()
{
	m_DefaultPSO.Destroy();
	m_DefaultRS.Destroy();
	m_BoxVertexBuffer.Destroy();
	m_BoxIndexBuffer.Destroy();
	//m_ObjPerObject.Destroy();
}