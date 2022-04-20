#pragma once
#include "PCH.h"
#include "CameraStorage.h"
#include "DataStruct.h"

using namespace DirectX;

class SkyPassStorage : public TSingleton<SkyPassStorage>
{
public:
	void Load()
	{
		Update();
	}

	void Update()
	{
		XMMATRIX proj = CameraStorage::GetInstance().Cameras["MainCamera"]->GetProjMatrix();
		XMMATRIX view = CameraStorage::GetInstance().Cameras["MainCamera"]->GetViewMatrix();

		XMVECTOR viewDet = XMMatrixDeterminant(view);
		XMVECTOR projDet = XMMatrixDeterminant(proj);

		XMMATRIX invView = XMMatrixInverse(&viewDet, view);
		XMMATRIX invProj = XMMatrixInverse(&projDet, proj);

		XMStoreFloat4x4(&SkyboxVSCB.ProjInverse, XMMatrixTranspose(invProj));
		XMStoreFloat4x4(&SkyboxVSCB.ViewInverse, XMMatrixTranspose(invView));

		SkyboxPSCB.TextureLevel = 0.0f;
	}

	SkyboxVSCB SkyboxVSCB;
	SkyboxPSCB SkyboxPSCB;
};
