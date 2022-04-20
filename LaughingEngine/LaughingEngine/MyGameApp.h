#pragma once
#include "GameCore.h"
#include "ECS/EntityAdmin.h"

class MyGameApp : public Game::IGameApp
{
public:
	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Draw() override;
	virtual void Shutdown() override;

private:
	// ECS - World
	std::unique_ptr<EntityAdmin> m_EntityAdmin;

	std::vector<EntityID> m_Entities;
};