#pragma once

#include "../Math/Transform.h"

class Entity;
class Scene;

class Component
{
	friend class Entity;
public:
	Component();

	inline Entity* GetParent() const { return parent; }

	Transform GetWorldTransform();

	virtual void OnSpawned();
	virtual void OnDestroyed();
	virtual void OnUpdate(float deltaSeconds);

private:

	Entity* parent = nullptr;

	//Transform relativeTransform;
};