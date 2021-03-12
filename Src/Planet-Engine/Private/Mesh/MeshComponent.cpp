#include "Mesh/MeshComponent.h"

#include "PlanetEngine.h"
#include "Render/RenderQueue.h"
#include "Mesh/Mesh.h"
#include "Entity/Entity.h"
#include "Material/Material.h"

MeshComponent::MeshComponent(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) :
    mMesh(mesh), mMaterial(material)
{
}

void MeshComponent::SetVisibility(bool newVisibility)
{
    if (mVisible == newVisibility) return;

    RenderQueue* queue = PlanetEngine::Get()->GetRenderQueue();

    mVisible = newVisibility;
    if (mVisible)
    {
        queue->AddMesh(this);
    }
    else
    {
        queue->RemoveMesh(this);
    }
}

void MeshComponent::OnSpawned()
{
    Component::OnSpawned();

    SetVisibility(true);
}

void MeshComponent::OnDestroyed()
{
    Component::OnDestroyed();

    SetVisibility(false);
}
