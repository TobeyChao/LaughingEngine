#pragma once
#include "PCH.h"
#include "Camera.h"
#include "Display.h"
#include <DirectXMath.h>

class CameraStorage : public TSingleton<CameraStorage>
{
public:
	void Load()
	{
		Cameras["MainCamera"] = std::make_unique<Camera>();
		Cameras["MainCamera"]->SetPosition3f({ 0, 3, -100 });
		Cameras["MainCamera"]->SetLens(XM_PIDIV4, (float)Graphics::g_DisplayWidth / Graphics::g_DisplayHeight, 0.1f, 1000.0f);
		Cameras["MainCamera"]->ComputeInfo();
	}

	std::unordered_map<std::string, std::unique_ptr<Camera>> Cameras;
	float Yaw = 0;
	float Pitch = XMConvertToRadians(15);
	float CamMoveSpeed = 20.f;
	XMFLOAT2 LastMousePos;
};
