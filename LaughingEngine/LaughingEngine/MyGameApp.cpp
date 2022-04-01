#include "MyGameApp.h"
#include "BufferManager.h"
#include "GraphicsCore.h"
#include "CommandContext.h"
#include "CommandListManager.h"
#include "GraphicsCommon.h"
#include "UploadBuffer.h"
#include "Display.h"
#include "GameTimer.h"
#include "GeometryGenerator.h"

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
	// ¼ÓÔØÍ¼Æ¬
	m_TextureHeap.Create(L"Scene Texture Descriptors", D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 4096);
	m_SamplerHeap.Create(L"Scene Sampler Descriptors", D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 2048);

	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_TextureHeap.Alloc(1);

	m_TextureReferences.resize(1);
	m_TextureReferences[0] = TextureManager::LoadDDSFromFile(L"../Assets/Textures/WoodCrate02.dds");
	D3D12_CPU_DESCRIPTOR_HANDLE SourceTextures[] =
	{
		m_TextureReferences[0].GetSRV()
	};

	Graphics::g_Device->CopyDescriptorsSimple(1, handle, SourceTextures[0], D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	m_DefaultRS.CreateFromMemory(L"MyGameApp::m_DefaultRS", g_pLightRS, sizeof(g_pLightRS));

	m_DefaultPSO.SetRootSignature(m_DefaultRS);
	m_DefaultPSO.SetRasterizerState(RasterizerDefault);
	m_DefaultPSO.SetBlendState(CD3DX12_BLEND_DESC(D3D12_DEFAULT));
	m_DefaultPSO.SetDepthStencilState(CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT));
	m_DefaultPSO.SetSampleMask(UINT_MAX);
	m_DefaultPSO.SetInputLayout(_countof(inputElementDesc), inputElementDesc);
	m_DefaultPSO.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	m_DefaultPSO.SetVertexShader(g_pLightVS, sizeof(g_pLightVS));
	m_DefaultPSO.SetPixelShader(g_pLightPS, sizeof(g_pLightPS));
	m_DefaultPSO.SetRenderTargetFormat(g_DefaultHdrColorFormat, g_DefaultDepthStencilFormat);
	m_DefaultPSO.Finalize();

	GeometryGenerator geoGen;
	GeometryGenerator::MeshData model = geoGen.CreateBox(1.0f, 1.0f, 1.0f, 0);
	std::vector<Vertex> vertices(model.Vertices.size());

	for (size_t i = 0; i < model.Vertices.size(); ++i)
	{
		auto& p = model.Vertices[i];
		vertices[i].Position = { p.Position.x, p.Position.y, p.Position.z };
		vertices[i].TexCoord = p.TexC;
		vertices[i].Normal = p.Normal;
		vertices[i].Color = XMFLOAT4(DirectX::Colors::White);
	}

	std::vector<std::uint16_t> indices;
	indices.insert(indices.end(), std::begin(model.GetIndices16()), std::end(model.GetIndices16()));

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
	m_Cameras["MainCamera"]->SetPosition3f({ 0, 3, -7 });
	m_Cameras["MainCamera"]->SetLens(XM_PIDIV4, (float)Graphics::g_DisplayWidth / Graphics::g_DisplayHeight, 0.1f, 100.0f);
	m_Cameras["MainCamera"]->Pitch(0.3f);
	m_Cameras["MainCamera"]->ComputeInfo();

	auto world = XMMatrixRotationY(XM_PIDIV4) * XMMatrixIdentity();
	XMStoreFloat4x4(&m_ObjPerObject.World, XMMatrixTranspose(world));

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

	m_MainPassCB.Lights[0].Direction = { -1.0f, -2.0f, 1.0f };
	m_MainPassCB.Lights[0].Strength = { 0.8f, 0.8f, 0.8f };
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

	// Pass
	m_Theta += (float)GameTimer::DeltaTime();
	if (m_Theta > XM_2PI)
	{
		m_Theta = 0.0f;
	}
	m_MainPassCB.Lights[0].Direction = { sinf(m_Theta), -2.0f, cosf(m_Theta) };
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
	Context.SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_TextureHeap.GetDescriptorHeapPointer());
	Context.SetDescriptorTable(0, m_TextureHeap[0]);
	Context.SetDynamicConstantBufferView(1, sizeof(m_MainPassCB), &m_MainPassCB);
	Context.SetDynamicConstantBufferView(2, sizeof(m_ObjPerObject), &m_ObjPerObject);
	Context.SetIndexBuffer(m_BoxIndexBufferView);
	Context.SetVertexBuffer(0, m_BoxVertexBufferView);
	Context.DrawIndexedInstanced(36, 1, 0, 0, 0);
	Context.Finish();
}

void MyGameApp::Shutdown()
{
	m_TextureReferences.clear();
	m_DefaultPSO.Destroy();
	m_DefaultRS.Destroy();
	m_BoxVertexBuffer.Destroy();
	m_BoxIndexBuffer.Destroy();
	m_TextureHeap.Destroy();
	m_SamplerHeap.Destroy();
}