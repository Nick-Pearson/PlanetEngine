#include "SkyDome.h"
#include "../Mesh/MeshComponent.h"
#include "../Mesh/Primitives.h"

SkyDome::SkyDome()
{
	std::shared_ptr<Mesh> mesh = Primitives::SubdivisionSurfacesElipsoid(Elipsoid(1.0f), 5);
	mesh->FlipFaces();
	domeMesh = AddComponent<MeshComponent>(mesh, "SkySphere.hlsl");
	domeMesh->SetUseDepthBuffer(false);
	domeMesh->SetUseWorldMatrix(false);
}
