#include "PCH.h"
#include "MyGameApp.h"
#include "CameraSystem.h"
#include "RenderSystem.h"
#include "CameraStorage.h"
#include "GeometriesStorage.h"
#include "LightSystem.h"
#include "MainPassStorage.h"
#include "MaterialManager.h"
#include "PSOStorage.h"
#include "RenderTargetStorage.h"
#include "ShaderManager.h"
#include "SkyPassStorage.h"
#include "TextureStorage.h"

using namespace Graphics;

void MyGameApp::Initialize()
{
	CameraStorage* cameraStorage = new CameraStorage();
	GeometriesStorage* geometriesStorage = new GeometriesStorage();
	LightStorage* lightManager = new LightStorage();
	MainPassStorage* mainPassStorage = new MainPassStorage();
	PSOStorage* psoStorage = new PSOStorage();
	RenderTargetStorage* renderTargetStorage = new RenderTargetStorage();
	SkyPassStorage* skyPassStorage = new SkyPassStorage();
	TextureStorage* textureStorage = new TextureStorage();

	ShaderManager& shaderManager = ShaderManager::GetInstance();
	shaderManager.RegisterShader(L"Light", L"Assets\\CompiledShaders\\LightVS.cso", L"Assets\\CompiledShaders\\LightPS.cso");
	shaderManager.RegisterShader(L"Color", L"Assets\\CompiledShaders\\ColorVS.cso", L"Assets\\CompiledShaders\\ColorPS.cso");
	shaderManager.RegisterShader(L"Pbr", L"Assets\\CompiledShaders\\PbrVS.cso", L"Assets\\CompiledShaders\\PbrPS.cso");
	shaderManager.RegisterShader(L"Sky", L"Assets\\CompiledShaders\\SkyVS.cso", L"Assets\\CompiledShaders\\SkyPS.cso");

	cameraStorage->Load();
	geometriesStorage->Load();
	mainPassStorage->Load();
	psoStorage->Load();
	renderTargetStorage->Load();
	skyPassStorage->Load();
	textureStorage->Load();

	m_EntityAdmin = std::make_unique<EntityAdmin>();
	using T = TypeList<Transform, MeshRenderer>;
	m_EntityAdmin->RegisterArchetype<T>();
	m_EntityAdmin->RegisterSystem<LightSystem>();
	m_EntityAdmin->RegisterSystem<CameraSystem>();
	m_EntityAdmin->RegisterSystem<RenderSystem>();

	// Create Entities
	{
		RenderLayer layer = RenderLayer::Geometry;

		{
			MeshGeometry* mesh = GeometriesStorage::GetInstance().Geometries[L"Cerberus"].get();
			for (auto& pair : mesh->DrawArgs)
			{
				EntityID id = m_EntityAdmin->CreateEntity<T>();

				MeshRenderer* ItemRenderer = m_EntityAdmin->SetComponentData<MeshRenderer>(id);
				Transform* ItemTransform = m_EntityAdmin->SetComponentData<Transform>(id);

				SubmeshGeometry* subMesh = &pair.second;
				XMMATRIX World = XMMatrixScaling(0.02f, 0.02f, 0.02f) * (XMMatrixRotationX(XM_PIDIV2));
				XMStoreFloat4x4(&ItemTransform->World, XMMatrixTranspose(World));
				ItemRenderer->Mesh = mesh;
				ItemRenderer->SubMesh = subMesh;
				ItemRenderer->RenderLayer = layer;
				ItemRenderer->Material = MaterialManager::GetMaterial(L"Assets\\Materials\\GunMatPbr.json");

				m_Entities.push_back(id);
			}
		}

		{
			MeshGeometry* mesh = GeometriesStorage::GetInstance().Geometries[L"Plane"].get();
			for (auto& pair : mesh->DrawArgs)
			{
				EntityID id = m_EntityAdmin->CreateEntity<T>();

				MeshRenderer* ItemRenderer = m_EntityAdmin->SetComponentData<MeshRenderer>(id);
				Transform* ItemTransform = m_EntityAdmin->SetComponentData<Transform>(id);

				SubmeshGeometry* subMesh = &pair.second;
				XMMATRIX World = XMMatrixTranslation(0, -1.0f, 0);
				XMStoreFloat4x4(&ItemTransform->World, XMMatrixTranspose(World));
				ItemRenderer->Mesh = mesh;
				ItemRenderer->SubMesh = subMesh;
				ItemRenderer->RenderLayer = layer;
				ItemRenderer->Material = MaterialManager::GetMaterial(L"Assets\\Materials\\blackrock.json");

				m_Entities.push_back(id);
			}
		}
	}
}

void MyGameApp::Update()
{
	MainPassStorage::GetInstance().Update();
	SkyPassStorage::GetInstance().Update();

	m_EntityAdmin->Update(GameTimer::DeltaTime());
}

void MyGameApp::Draw()
{
}

void MyGameApp::Shutdown()
{
	m_EntityAdmin->Shutdown();
	MaterialManager::Shutdown();

	CameraStorage::GetInstance().Shutdown();
	GeometriesStorage::GetInstance().Shutdown();
	LightStorage::GetInstance().Shutdown();
	MainPassStorage::GetInstance().Shutdown();
	PSOStorage::GetInstance().Shutdown();
	RenderTargetStorage::GetInstance().Shutdown();
	SkyPassStorage::GetInstance().Shutdown();
	TextureStorage::GetInstance().Shutdown();
}