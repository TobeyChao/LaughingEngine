#pragma once
#include "LightStorage.h"
#include "ECS/System.h"

class LightSystem : public System<>
{
public:
	void Update(float deltaTime) override
	{
		//float& Theta = LightStorage::GetInstance().Theta;
		//Theta += deltaTime;
		//if (Theta > XM_2PI)
		//{
		//	Theta = 0.0f;
		//}
	}
};
