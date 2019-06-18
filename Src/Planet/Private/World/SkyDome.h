#pragma once

#include "../Entity/Entity.h"

class MeshComponent;

class SkyDome: public Entity
{
public:
	SkyDome();

	void OnUpdate(float deltaSeconds) override;

private:
	std::shared_ptr<MeshComponent> mDomeMesh;

	Quaternion mSunRotation;
	Vector mSunColour = Vector{ 1.0f, 1.0f, 1.0f };
	float mSunSkyStrength = 2.0f;

	float mSunSpeed = 5.0f;
};