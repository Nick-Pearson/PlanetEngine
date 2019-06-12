#include "FlyCam.h"
#include "../Input/InputManager.h"
#include "../PlanetEngine.h"
#include "../World/CameraComponent.h"

FlyCam::FlyCam()
{
	camera = AddComponent<CameraComponent>();
}

void FlyCam::OnUpdate(float deltaSeconds)
{
	InputManager* mgr = PlanetEngine::Get()->GetInputManager();

	if (!mgr) return;

	Vector movement;

	if (mgr->GetIsKeyDown(KeyCode::W))
	{
		movement += Vector(0.0f, 0.0f, 1.0f);
	}
	else if(mgr->GetIsKeyDown(KeyCode::S))
	{
		movement += Vector(0.0f, 0.0f, -1.0f);
	}

	if (mgr->GetIsKeyDown(KeyCode::D))
	{
		movement += Vector(1.0f, 0.0f, 0.0f);
	}
	else if (mgr->GetIsKeyDown(KeyCode::A))
	{
		movement += Vector(-1.0f, 0.0f, 0.0f);
	}

	Translate(movement * deltaSeconds * speed);
}
