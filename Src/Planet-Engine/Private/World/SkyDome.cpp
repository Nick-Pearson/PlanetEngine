#include "SkyDome.h"
#include "PlanetEngine.h"
#include "Mesh/MeshComponent.h"
#include "Material/Material.h"
#include "../Mesh/Primitives.h"
#include "Render/Renderer.h"

SkyDome::SkyDome()
{
    auto mesh = Primitives::SubdivisionSurfacesElipsoid(Elipsoid(1.0f), 3);
    mesh->FlipFaces();
    auto skyMaterial = std::make_shared<Material>("SkySphere.hlsl");

    auto domeMesh = AddComponent<MeshComponent>(mesh, skyMaterial);
    domeMesh->render_config_.use_depth_buffer = false;
    domeMesh->render_config_.use_world_matrix = false;

    mTimeOfDay = new TimeOfDay{};
}

void SkyDome::OnUpdate(float deltaSeconds)
{
    Entity::OnUpdate(deltaSeconds);

    mTimeOfDay->Update(deltaSeconds);

    Vector sunDirection = mTimeOfDay->CalculateSunDirection();

    auto renderer = PlanetEngine::Get()->GetRenderer();
    renderer->UpdateWorldBuffer(WorldBufferData(sunDirection, mSunSkyStrength, mSunColour));
}
