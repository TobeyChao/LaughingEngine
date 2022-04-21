#include "PCH.h"
#include "MyGameApp.h"
#include "CameraSystem.h"
#include "RenderSystem.h"
#include "CameraStorage.h"
#include "GeometriesStorage.h"
#include "LightSystem.h"
#include "MainPassStorage.h"
#include "PSOStorage.h"
#include "RenderTargetStorage.h"
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
				ItemRenderer->TextureIndex = 6;

				m_Entities.push_back(id);
			}
		}

		{
			MeshGeometry* mesh = GeometriesStorage::GetInstance().Geometries[L"Box"].get();
			for (auto& pair : mesh->DrawArgs)
			{
				EntityID id = m_EntityAdmin->CreateEntity<T>();

				MeshRenderer* ItemRenderer = m_EntityAdmin->SetComponentData<MeshRenderer>(id);
				Transform* ItemTransform = m_EntityAdmin->SetComponentData<Transform>(id);

				SubmeshGeometry* subMesh = &pair.second;
				XMMATRIX World = XMMatrixTranslation(0, 0.0f, 1.2f);
				XMStoreFloat4x4(&ItemTransform->World, XMMatrixTranspose(World));
				ItemRenderer->Mesh = mesh;
				ItemRenderer->SubMesh = subMesh;
				ItemRenderer->RenderLayer = layer;
				ItemRenderer->TextureIndex = 0;

				m_Entities.push_back(id);
			}
		}
	}
}

void MyGameApp::Update()
{
	MainPassStorage::GetInstance().Update();
	SkyPassStorage::GetInstance().Update();

	m_EntityAdmin->Update((float)GameTimer::DeltaTime());
}

void MyGameApp::Draw()
{
}

void MyGameApp::Shutdown()
{
}