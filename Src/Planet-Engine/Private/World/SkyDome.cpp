#include "SkyDome.h"
#include "PlanetEngine.h"
#include "../Mesh/MeshComponent.h"
#include "../Mesh/Primitives.h"
#include "../Material/Material.h"
#include "Render/RenderSystem.h"
#include "Render/Renderer.h"

SkyDome::SkyDome()
{
    auto mesh = Primitives::SubdivisionSurfacesElipsoid(Elipsoid(1.0f), 3);
    mesh->FlipFaces();
    auto skyMaterial = std::make_shared<Material>("SkySphere.hlsl");

    auto domeMesh = AddComponent<MeshComponent>(mesh, skyMaterial);
    domeMesh->SetUseDepthBuffer(false);
    domeMesh->SetUseWorldMatrix(false);

    mTimeOfDay = new TimeOfDay{};
}

void SkyDome::OnUpdate(float deltaSeconds)
{
    Entity::OnUpdate(deltaSeconds);

    mTimeOfDay->Update(deltaSeconds);

    Vector sunDirection = mTimeOfDay->CalculateSunDirection();

    auto renderer = PlanetEngine::Get()->GetRenderSystem()->GetRenderer();
    renderer->UpdateWorldBuffer(WorldBufferData(sunDirection, mSunSkyStrength, mSunColour));
}
