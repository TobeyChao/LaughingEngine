#pragma once
#include "PCH.h"
#include "CameraStorage.h"
#include "ECS/System.h"
#include "InputSystem.h"

class CameraSystem : public System<>
{
public:
	virtual void Update(float deltaTime) override
	{
		Camera* camera = CameraStorage::GetInstance().Cameras["MainCamera"].get();
		float camMoveSpeed = CameraStorage::GetInstance().CamMoveSpeed;
		XMFLOAT2& lastMousePos = CameraStorage::GetInstance().LastMousePos;
		float& yaw = CameraStorage::GetInstance().Yaw;
		float& pitch = CameraStorage::GetInstance().Pitch;

		if (InputSystem::GetInstance().KeyIsPressed(KeyCode::KEYCODE_W))
			camera->Walk(deltaTime * camMoveSpeed);
		if (InputSystem::GetInstance().KeyIsPressed(KeyCode::KEYCODE_S))
			camera->Walk(deltaTime * -camMoveSpeed);
		if (InputSystem::GetInstance().KeyIsPressed(KeyCode::KEYCODE_A))
			camera->Strafe(deltaTime * -camMoveSpeed);
		if (InputSystem::GetInstance().KeyIsPressed(KeyCode::KEYCODE_D))
			camera->Strafe(deltaTime * camMoveSpeed);

		const auto& [x, y] = InputSystem::GetInstance().GetPos();

		if (InputSystem::GetInstance().LeftIsPressed())
		{
			// Make each pixel correspond to a quarter of a degree.
			float dx = XMConvertToRadians(0.15f * (x - lastMousePos.x));
			float dy = XMConvertToRadians(0.15f * (y - lastMousePos.y));

			// Update angles based on input to orbit camera around box.
			yaw += dx;
			pitch += dy;
		}

		lastMousePos.x = static_cast<float>(x);
		lastMousePos.y = static_cast<float>(y);

		camera->Pitch(pitch);
		camera->Yaw(yaw);
		camera->ComputeInfo();
	}
};
