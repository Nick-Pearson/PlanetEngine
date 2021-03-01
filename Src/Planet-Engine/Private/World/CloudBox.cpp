#include "CloudBox.h"
#include "PlanetEngine.h"
#include "../Mesh/MeshComponent.h"
#include "../Mesh/Primitives.h"
#include "../Material/Material.h"

CloudBox::CloudBox()
{
    std::shared_ptr<Mesh> mesh = Primitives::Cube(1.0f);
    mesh->Scale(Vector{50.0f, 1.0f, 50.0f});
    std::shared_ptr<Material> cloudMaterial = std::make_shared<Material>("CloudsShader.hlsl");
    cloudMaterial->EnableAlphaBlending();
    mCloudMesh = AddComponent<MeshComponent>(mesh, cloudMaterial);
    mCloudMesh->SetUseDepthBuffer(false);
    mCloudMesh->SetUseWorldMatrix(false);
    Translate(Vector{0.0f, -4.0f, 0.0f});
}
