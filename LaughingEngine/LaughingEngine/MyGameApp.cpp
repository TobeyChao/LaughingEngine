#include "MyGameApp.h"
#include "TextureManager.h"
#include "BufferManager.h"
#include "CommandContext.h"
#include "CommandListManager.h"
#include "GraphicsCommon.h"
#include "UploadBuffer.h"
#include "Display.h"
#include <array>

// Shader
#include "CompiledShaders/ColorVS.h"
#include "CompiledShaders/ColorPS.h"
#include "CompiledShaders/ColorRS.h"

using namespace Graphics;

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

struct ObjectConstants
{
	XMFLOAT4X4 WorldViewProj;
};

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

	m_DefaultRS.CreateFromMemory(L"MyGameApp::m_DefaultRS", g_pColorRS, sizeof(g_pColorRS));

	m_DefaultPSO = new GraphicsPiplelineState(L"MyGameApp::m_DefaultPSO");
	m_DefaultPSO->SetRootSignature(m_DefaultRS);
	m_DefaultPSO->SetRasterizerState(RasterizerDefault);
	m_DefaultPSO->SetBlendState(CD3DX12_BLEND_DESC(D3D12_DEFAULT));
	m_DefaultPSO->SetDepthStencilState(CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT));
	m_DefaultPSO->SetSampleMask(UINT_MAX);
	m_DefaultPSO->SetInputLayout(2, inputElementDesc);
	m_DefaultPSO->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	m_DefaultPSO->SetVertexShader(g_pColorVS, sizeof(g_pColorVS));
	m_DefaultPSO->SetPixelShader(g_pColorPS, sizeof(g_pColorPS));
	m_DefaultPSO->SetRenderTargetFormat(g_DefaultHdrColorFormat, g_DefaultDepthStencilFormat);
	m_DefaultPSO->Finalize();

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
	m_Cameras["MainCamera"]->SetPosition3f({ 0, 2, -10 });
	m_Cameras["MainCamera"]->SetLens(XM_PIDIV4, (float)Graphics::g_DisplayWidth / Graphics::g_DisplayHeight, 0.1f, 100.0f);
	m_Cameras["MainCamera"]->ComputeInfo();

	auto world = XMMatrixIdentity();
	auto view = m_Cameras["MainCamera"]->GetViewMatrix();
	auto proj = m_Cameras["MainCamera"]->GetProjMatrix();
	auto worldViewProj = world * view * proj;
	ObjectConstants objConstants;
	XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(worldViewProj));
	m_ObjPerObject.Create(L"m_ObjPerObject", sizeof(ObjectConstants), 1, &objConstants);
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
	context.SetRootSignature(m_DefaultRS);
	context.SetPipelineState(*m_DefaultPSO);
	context.TransitionResource(g_SceneColorBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, true);
	context.SetRenderTarget(g_SceneColorBuffer.GetRTV(), g_SceneDepthBuffer.GetDSV());
	context.ClearColor(g_SceneColorBuffer, &scissor);
	context.ClearDepth(g_SceneDepthBuffer);
	context.SetViewportAndScissorRect(viewport, scissor);
	context.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context.SetConstantBuffer(0, m_ObjPerObject.GetGpuAddress());
	context.SetIndexBuffer(m_BoxIndexBufferView);
	context.SetVertexBuffer(0, m_BoxVertexBufferView);
	context.DrawIndexedInstanced(36, 1, 0, 0, 0);
	context.Finish();
}

void MyGameApp::Shutdown()
{
	m_DefaultPSO->Destroy();
	m_DefaultRS.Destroy();
	m_BoxVertexBuffer.Destroy();
	m_BoxIndexBuffer.Destroy();
	m_ObjPerObject.Destroy();
}