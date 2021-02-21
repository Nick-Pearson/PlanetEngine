#pragma once

#include "../Entity/Entity.h"

class MeshComponent;

class CloudBox: public Entity
{
public:
	CloudBox();

private:
	std::shared_ptr<MeshComponent> mCloudMesh;
};