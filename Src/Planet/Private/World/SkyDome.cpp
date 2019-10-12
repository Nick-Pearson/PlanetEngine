#include "SkyDome.h"
#include "../Mesh/MeshComponent.h"
#include "../Mesh/Primitives.h"
#include "../Renderer/D3D11/D3DRenderer.h"
#include "../PlanetEngine.h"
#include "../Renderer/RenderManager.h"

SkyDome::SkyDome()
{
	std::shared_ptr<Mesh> mesh = Primitives::SubdivisionSurfacesElipsoid(Elipsoid(1.0f), 5);
	mesh->FlipFaces();
	mDomeMesh = AddComponent<MeshComponent>(mesh, "SkySphere.hlsl");
	mDomeMesh->SetUseDepthBuffer(false);
	mDomeMesh->SetUseWorldMatrix(false);
}

void SkyDome::OnUpdate(float deltaSeconds)
{
	Entity::OnUpdate(deltaSeconds);

	mSunRotation += Vector(mSunSpeed * deltaSeconds, 0.0f, 0.0f);

	// maybe dont do this all the time?
	PlanetEngine::Get()->GetRenderManager()->GetRenderer()->UpdateWorldBuffer(WorldBufferData(mSunRotation * Vector{0.0f, 1.0f, 0.0f}, mSunSkyStrength, mSunColour));
}
