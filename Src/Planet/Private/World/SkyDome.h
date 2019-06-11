#pragma once

#include "../Entity/Entity.h"

class MeshComponent;

class SkyDome: public Entity
{
public:
	SkyDome();

private:
	std::shared_ptr<MeshComponent> domeMesh;
};