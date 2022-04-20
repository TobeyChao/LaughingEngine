#include "PCH.h"
#include "MyGameApp.h"
#include "BufferManager.h"
#include "GraphicsCore.h"
#include "CommandListManager.h"
#include "GraphicsCommon.h"
#include "UploadBuffer.h"
#include "Display.h"
#include "GameTimer.h"
#include "GeometryGenerator.h"
#include "InputSystem.h"

// Shader
#include "CompiledShaders/CommonRS.h"

#include "CompiledShaders/LightVS.h"
#include "CompiledShaders/LightPS.h"

#include "CompiledShaders/SkyVS.h"
#include "CompiledShaders/SkyPS.h"

#include <json.hpp>

#include <assimp/Importer.hpp>
#include <assimp/PostProcess.h>
#include <assimp/Scene.h>

using namespace Graphics;

void MyGameApp::Initialize()
{
	// ¼ÓÔØÍ¼Æ¬
	m_TextureHeap.Create(L"Scene Texture Descriptors", D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 4096);

	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_TextureHeap.Alloc(2);

	m_TextureReferences.resize(6);
	m_TextureReferences[0] = TextureManager::LoadDDSFromFile(L"../Assets/Textures/WoodCrate02.dds");
	m_TextureReferences[1] = TextureManager::LoadDDSFromFile(L"../Assets/Textures/SkyBox.dds");
	m_TextureReferences[2] = TextureManager::LoadDDSFromFile(L"../Assets/Textures/outBrdf.dds");
	m_TextureReferences[3] = TextureManager::LoadDDSFromFile(L"../Assets/Textures/outDiffuseHDR.dds");
	m_TextureReferences[4] = TextureManager::LoadDDSFromFile(L"../Assets/Textures/outEnvHDR.dds");
	m_TextureReferences[5] = TextureManager::LoadDDSFromFile(L"../Assets/Textures/outSpecularHDR.dds");

	uint32_t DestCount = 2;
	uint32_t SourceCounts[] = { 1, 1 };

	D3D12_CPU_DESCRIPTOR_HANDLE SourceTextures[] =
	{
		m_TextureReferences[0].GetSRV(),
		m_TextureReferences[4].GetSRV()
	};

	g_Device->CopyDescriptors(1, &handle, &DestCount, DestCount, SourceTextures, SourceCounts, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


	D3D12_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	//m_DefaultRS.Reset(4, 2);
	//m_DefaultRS[0].InitAsDescriptorTable(3, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0);
	//m_DefaultRS[1].InitAsConstantBufferView(0, 0);
	//m_DefaultRS[2].InitAsConstantBufferView(1, 0);
	//m_DefaultRS[3].InitAsDescriptorTable(2, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 10);
	//m_DefaultRS.InitStaticSampler(SamplerLinearClamp, 0);
	//m_DefaultRS.InitStaticSampler(SamplerPointClamp, 1);
	//m_DefaultRS.Finalize(L"MyGameApp::m_DefaultRS", D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	m_DefaultRS.CreateFromMemory(L"MyGameApp::m_DefaultRS", g_pCommonRS, sizeof(g_pCommonRS));

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

	m_SkyPSO = m_DefaultPSO;
	m_SkyPSO.SetDepthStencilState(DepthStateTestLessEqual);
	m_SkyPSO.SetRasterizerState(RasterizerTwoSided);
	m_SkyPSO.SetVertexShader(g_pSkyVS, sizeof(g_pSkyVS));
	m_SkyPSO.SetPixelShader(g_pSkyPS, sizeof(g_pSkyPS));
	m_SkyPSO.SetInputLayout(0, nullptr);
	m_SkyPSO.Finalize();

	{
		Assimp::Importer loader;
		aiMaterial* material = nullptr;
		aiString path;

		const aiScene* scene = loader.ReadFile("../Assets/Meshes/cerberus.fbx",
			aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded);

		auto geo = std::make_unique<MeshGeometry>();
		geo->Name = Utility::UTF8ToWideString(scene->mName.C_Str());

		std::vector<Vertex> vertices;
		std::vector<std::uint16_t> indices;

		size_t startIndexLocation = 0;
		size_t baseVertexLocation = 0;
		for (unsigned i = 0; i < scene->mNumMeshes; i++)
		{
			aiMesh* aimesh = scene->mMeshes[i];

			material = scene->mMaterials[aimesh->mMaterialIndex];

			material->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &path);

			std::vector<Vertex> subVertices(aimesh->mNumVertices);
			for (size_t i = 0; i < aimesh->mNumVertices; ++i)
			{
				auto& p = aimesh->mVertices[i];
				int uvChannelNum = aimesh->GetNumUVChannels();
				if (uvChannelNum >= 1)
				{
					auto& texC = aimesh->mTextureCoords[0][i];
					subVertices[i].TexCoord = XMFLOAT2{ texC.x, texC.y };
				}
				int colorChannelNum = aimesh->GetNumColorChannels();
				if (colorChannelNum >= 1)
				{
					auto& color = aimesh->mColors[0][i];
					subVertices[i].Color = { color.r, color.g, color.b, color.a };
				}
				else
				{
					subVertices[i].Color = XMFLOAT4(DirectX::Colors::White);
				}
				auto& normal = aimesh->mNormals[i];
				subVertices[i].Position = { p.x, p.y, p.z };
				subVertices[i].Normal = { normal.x, normal.y, normal.z };
			}
			std::vector<std::uint16_t> subIndices;
			for (unsigned k = 0; k < aimesh->mNumFaces; k++)
			{
				const struct aiFace* face = &aimesh->mFaces[k];
				for (unsigned m = 0; m < face->mNumIndices; m++)
				{
					int index = face->mIndices[m];
					subIndices.push_back(index);
				}
			}

			SubmeshGeometry submesh;
			submesh.IndexCount = (UINT)subIndices.size();
			submesh.StartIndexLocation = (UINT)startIndexLocation;
			submesh.BaseVertexLocation = (INT)baseVertexLocation;

			geo->DrawArgs[Utility::UTF8ToWideString(aimesh->mName.C_Str())] = submesh;

			baseVertexLocation += subVertices.size();
			startIndexLocation += subIndices.size();

			vertices.insert(vertices.end(), subVertices.begin(), subVertices.end());
			indices.insert(indices.end(), subIndices.begin(), subIndices.end());
		}

		const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
		const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

		ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
		CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

		ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
		CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

		geo->VertexBufferGPU.Create(L"fbx-VertexBufferGPU", sizeof(Vertex), (UINT)vertices.size(), geo->VertexBufferCPU->GetBufferPointer());
		geo->IndexBufferGPU.Create(L"fbx-IndexBufferGPU", sizeof(std::uint16_t), (UINT)indices.size(), geo->IndexBufferCPU->GetBufferPointer());

		m_Geometries[geo->Name] = std::move(geo);

		loader.FreeScene();
	}

	{
		auto geo = std::make_unique<MeshGeometry>();
		geo->Name = L"Box";

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

		//{
		//	UploadBuffer uploadBufferV;
		//	uploadBufferV.Create(L"Temp uploadBufferV", vbByteSize);
		//	memcpy(uploadBufferV.Map(), vertices.data(), vbByteSize);
		//	uploadBufferV.Unmap();

		//	UploadBuffer uploadBufferI;
		//	uploadBufferI.Create(L"Temp uploadBufferI", ibByteSize);
		//	memcpy(uploadBufferI.Map(), indices.data(), ibByteSize);
		//	uploadBufferI.Unmap();

		//	geo->VertexBufferGPU.Create(L"MyGameApp::m_BoxVertexBuffer", sizeof(Vertex), (UINT)vertices.size(), uploadBufferV, 0);
		//	geo->IndexBufferGPU.Create(L"MyGameApp::m_BoxIndexBuffer", sizeof(sizeof(std::uint16_t)), (UINT)indices.size(), uploadBufferI, 0);
		//}

		ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
		CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

		ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
		CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

		geo->VertexBufferGPU.Create(L"MyGameApp::m_BoxVertexBuffer", sizeof(Vertex), (UINT)vertices.size(), geo->VertexBufferCPU->GetBufferPointer());
		geo->IndexBufferGPU.Create(L"MyGameApp::m_BoxIndexBuffer", sizeof(sizeof(std::uint16_t)), (UINT)indices.size(), geo->IndexBufferCPU->GetBufferPointer());

		SubmeshGeometry submesh;
		submesh.IndexCount = (UINT)indices.size();
		submesh.StartIndexLocation = 0;
		submesh.BaseVertexLocation = 0;

		geo->DrawArgs[L"default"] = submesh;
		m_Geometries[geo->Name] = std::move(geo);
	}

	BuildRenderItem();

	m_Cameras["MainCamera"] = std::make_unique<Camera>();
	m_Cameras["MainCamera"]->SetPosition3f({ 0, 3, -100 });
	m_Cameras["MainCamera"]->SetLens(XM_PIDIV4, (float)Graphics::g_DisplayWidth / Graphics::g_DisplayHeight, 0.1f, 1000.0f);
	m_Cameras["MainCamera"]->ComputeInfo();

	auto world = XMMatrixRotationY(XM_PIDIV4) * XMMatrixIdentity();
	XMStoreFloat4x4(&m_ObjCB.World, XMMatrixTranspose(world));
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
	//m_Angle += (float)GameTimer::DeltaTime();
	//if (m_Angle > XM_2PI)
	//{
	//	m_Angle = 0.0f;
	//}
	auto world = XMMatrixScaling(0.4f, 0.4f, 0.4f) * XMMatrixRotationX(XM_PIDIV2) * XMMatrixRotationY(XM_PIDIV2) * XMMatrixIdentity();

	XMStoreFloat4x4(&m_AllRitems[L"Cerberus00_Fixed"].get()->ItemTransform.World, XMMatrixTranspose(world));
	XMStoreFloat4x4(&m_AllRitems[L"default"].get()->ItemTransform.World, XMMatrixTranspose(world));

	// Pass
	m_Theta += (float)GameTimer::DeltaTime();
	if (m_Theta > XM_2PI)
	{
		m_Theta = 0.0f;
	}
	m_MainPassCB.Lights[0].Direction = { sinf(m_Theta), -2.0f, cosf(m_Theta) };

	if (InputSystem::GetInstance().KeyIsPressed(KeyCode::KEYCODE_W))
		m_Cameras["MainCamera"]->Walk((float)GameTimer::DeltaTime() * m_CamMoveSpeed);
	if (InputSystem::GetInstance().KeyIsPressed(KeyCode::KEYCODE_S))
		m_Cameras["MainCamera"]->Walk((float)GameTimer::DeltaTime() * -m_CamMoveSpeed);
	if (InputSystem::GetInstance().KeyIsPressed(KeyCode::KEYCODE_A))
		m_Cameras["MainCamera"]->Strafe((float)GameTimer::DeltaTime() * -m_CamMoveSpeed);
	if (InputSystem::GetInstance().KeyIsPressed(KeyCode::KEYCODE_D))
		m_Cameras["MainCamera"]->Strafe((float)GameTimer::DeltaTime() * m_CamMoveSpeed);

	const auto& [x, y] = InputSystem::GetInstance().GetPos();

	if (InputSystem::GetInstance().LeftIsPressed())
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - m_LastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - m_LastMousePos.y));

		// Update angles based on input to orbit camera around box.
		m_Yaw += dx;
		m_Pitch += dy;
	}

	m_LastMousePos.x = (float)x;
	m_LastMousePos.y = (float)y;

	m_Cameras["MainCamera"]->Pitch(m_Pitch);
	m_Cameras["MainCamera"]->Yaw(m_Yaw);
	m_Cameras["MainCamera"]->ComputeInfo();

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

	XMStoreFloat4x4(&m_SkyboxVSCB.ProjInverse, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&m_SkyboxVSCB.ViewInverse, XMMatrixTranspose(invView));

	m_SkyboxPSCB.TextureLevel = 0.0f;
}

void MyGameApp::Draw()
{
	const D3D12_VIEWPORT& Viewport = m_MainViewport;
	const D3D12_RECT& Scissor = m_MainScissor;
	GraphicsContext& Context = GraphicsContext::Begin(L"Scene Render");

	Context.TransitionResource(g_SceneColorBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, true);
	Context.SetRenderTarget(g_SceneColorBuffer.GetRTV(), g_SceneDepthBuffer.GetDSV());
	Context.ClearColor(g_SceneColorBuffer, &Scissor);
	Context.ClearDepth(g_SceneDepthBuffer);

	DrawRenderItems(Context, Viewport, Scissor);
	DrawSkybox(Context, Viewport, Scissor);

	Context.Finish();
}

void MyGameApp::Shutdown()
{
	m_TextureReferences.clear();
	m_DefaultPSO.Destroy();
	m_SkyPSO.Destroy();
	m_DefaultRS.Destroy();
	m_TextureHeap.Destroy();
}

void MyGameApp::BuildRenderItem()
{
	for (const auto& iter : m_Geometries)
	{
		MeshGeometry* mesh = iter.second.get();

		for (auto& pair : mesh->DrawArgs)
		{
			RenderLayer layer = RenderLayer::Geometry;

			SubmeshGeometry* subMesh = &pair.second;

			auto renderItem = std::make_unique<RenderItem>();
			XMStoreFloat4x4(&renderItem->ItemTransform.World, XMMatrixIdentity());
			renderItem->ItemRenderer.Mesh = mesh;
			renderItem->ItemRenderer.SubMesh = subMesh;
			renderItem->ItemRenderer.RenderLayer = layer;

			m_RitemLayer[(uint8_t)layer].push_back(renderItem.get());
			m_AllRitems[pair.first] = std::move(renderItem);
		}
	}
	;
}

void MyGameApp::DrawRenderItems(GraphicsContext& Context, const D3D12_VIEWPORT& Viewport, const D3D12_RECT& Scissor)
{
	Context.SetRootSignature(m_DefaultRS);
	Context.SetPipelineState(m_DefaultPSO);
	Context.SetViewportAndScissorRect(Viewport, Scissor);
	Context.SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_TextureHeap.GetDescriptorHeapPointer());
	Context.SetDescriptorTable(0, m_TextureHeap[0]);
	Context.SetDynamicConstantBufferView(1, sizeof(m_MainPassCB), &m_MainPassCB);

	auto ritems = m_RitemLayer[(uint8_t)RenderLayer::Geometry];
	for (size_t i = 0; i < ritems.size(); ++i)
	{
		auto ri = ritems[i];

		Context.SetVertexBuffer(0, ri->ItemRenderer.Mesh->VertexBufferGPU.VertexBufferView(0));
		Context.SetIndexBuffer(ri->ItemRenderer.Mesh->IndexBufferGPU.IndexBufferView(0));
		Context.SetPrimitiveTopology(ri->ItemRenderer.Mesh->PrimitiveType);

		m_ObjCB.World = ri->ItemTransform.World;
		Context.SetDynamicConstantBufferView(2, sizeof(m_ObjCB), &m_ObjCB);

		Context.DrawIndexedInstanced(
			ri->ItemRenderer.SubMesh->IndexCount,
			1, ri->ItemRenderer.SubMesh->StartIndexLocation,
			ri->ItemRenderer.SubMesh->BaseVertexLocation, 0);
	}
}

void MyGameApp::DrawSkybox(GraphicsContext& Context, const D3D12_VIEWPORT& Viewport, const D3D12_RECT& Scissor)
{
	Context.SetRootSignature(m_DefaultRS);
	Context.SetPipelineState(m_SkyPSO);
	Context.SetViewportAndScissorRect(Viewport, Scissor);
	Context.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Context.SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_TextureHeap.GetDescriptorHeapPointer());
	Context.SetDynamicConstantBufferView(1, sizeof(m_SkyboxVSCB), &m_SkyboxVSCB);
	Context.SetDynamicConstantBufferView(2, sizeof(m_SkyboxPSCB), &m_SkyboxPSCB);
	Context.SetDescriptorTable(3, m_TextureHeap[1]);
	Context.DrawInstanced(3, 1, 0, 0);
}
