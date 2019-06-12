#pragma once

#include <memory>

#include "../Entity/Entity.h"

class CameraComponent;

class FlyCam : public Entity
{
public:
	FlyCam();

	inline std::shared_ptr<CameraComponent> GetCamera() const { return camera; }

	void OnUpdate(float deltaSeconds) override;

private:
	std::shared_ptr<CameraComponent> camera;

	float speed = 10.0f;
};