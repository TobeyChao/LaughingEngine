#pragma once
#include "DataStruct.h"
#include "GameTimer.h"
#include "LightStorage.h"
#include "CameraStorage.h"

using namespace DirectX;

class MainPassStorage : public TSingleton<MainPassStorage>
{
public:
	void Load()
	{
		MainPass = CpuBuffer::Create(1, sizeof(PassConstants));
		MainPassCB = static_cast<PassConstants*>(MainPass->GetData());

		Update();

		MainPassCB->RenderTargetSize = XMFLOAT2{ (float)Graphics::g_DisplayWidth, (float)Graphics::g_DisplayHeight };
		MainPassCB->InvRenderTargetSize = { 1.0f / (float)Graphics::g_DisplayWidth, 1.0f / (float)Graphics::g_DisplayHeight };
		MainPassCB->NearZ = 0.1f;
		MainPassCB->FarZ = 1000.0f;
		MainPassCB->AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };

	}

	void Update()
	{
		// Pass
		XMMATRIX proj = CameraStorage::GetInstance().Cameras["MainCamera"]->GetProjMatrix();
		XMMATRIX view = CameraStorage::GetInstance().Cameras["MainCamera"]->GetViewMatrix();

		XMMATRIX viewProj = XMMatrixMultiply(view, proj);
		XMVECTOR viewDet = XMMatrixDeterminant(view);
		XMVECTOR projDet = XMMatrixDeterminant(proj);
		XMVECTOR viewProjDet = XMMatrixDeterminant(viewProj);
		XMMATRIX invView = XMMatrixInverse(&viewDet, view);
		XMMATRIX invProj = XMMatrixInverse(&projDet, proj);
		XMMATRIX invViewProj = XMMatrixInverse(&viewProjDet, viewProj);
		//XMMATRIX shadowTransform = XMLoadFloat4x4(&mShadowTransform);

		XMStoreFloat4x4(&MainPassCB->View, XMMatrixTranspose(view));
		XMStoreFloat4x4(&MainPassCB->InvView, XMMatrixTranspose(invView));
		XMStoreFloat4x4(&MainPassCB->Proj, XMMatrixTranspose(proj));
		XMStoreFloat4x4(&MainPassCB->InvProj, XMMatrixTranspose(invProj));
		XMStoreFloat4x4(&MainPassCB->ViewProj, XMMatrixTranspose(viewProj));
		XMStoreFloat4x4(&MainPassCB->InvViewProj, XMMatrixTranspose(invViewProj));
		//XMStoreFloat4x4(&mMainPassCB.ShadowTransform, XMMatrixTranspose(shadowTransform));

		MainPassCB->EyePosW = CameraStorage::GetInstance().Cameras["MainCamera"]->GetPosition3f();

		MainPassCB->TotalTime = GameTimer::TotalTime();
		MainPassCB->DeltaTime = GameTimer::DeltaTime();

		MainPassCB->Lights[0].Direction = { sinf(LightStorage::GetInstance().Theta), -2.0f, cosf(LightStorage::GetInstance().Theta) };
		MainPassCB->Lights[0].Strength = { 0.8f, 0.8f, 0.8f };
	}
	
	void Shutdown()
	{
		CpuBuffer::Destroy(MainPass);
		MainPassCB = nullptr;
	}

	PassConstants* MainPassCB;
	CpuBuffer* MainPass;
};
