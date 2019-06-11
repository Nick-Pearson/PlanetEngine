#pragma once

#include "../Entity/Component.h"

class CameraComponent : public Component
{
public:
	//CameraComponent();

	float NearClip = 0.1f;
	float FarClip = 5000.0f;
};