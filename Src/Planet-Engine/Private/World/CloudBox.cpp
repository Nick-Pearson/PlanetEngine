#include "CloudBox.h"
#include "PlanetEngine.h"
#include "../Mesh/MeshComponent.h"
#include "../Mesh/Primitives.h"
#include "../Material/Material.h"

CloudBox::CloudBox()
{
    auto mesh = Primitives::Cube(1.0f);
    mesh->Scale(Vector{50.0f, 1.0f, 50.0f});
    auto cloudMaterial = std::make_shared<Material>("CloudsShader.hlsl");
    cloudMaterial->EnableAlphaBlending();
    auto cloudMesh = AddComponent<MeshComponent>(mesh, cloudMaterial);
    cloudMesh->render_config_.use_depth_buffer = false;
    cloudMesh->render_config_.use_world_matrix = false;
    Translate(Vector{0.0f, -4.0f, 0.0f});
}
