#include "Mesh/MeshComponent.h"

#include "PlanetEngine.h"
#include "Render/RenderQueue.h"
#include "Mesh/Mesh.h"
#include "Entity/Entity.h"
#include "Material/Material.h"

MeshComponent::MeshComponent(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) :
    mesh_(mesh), material_(material)
{
}

void MeshComponent::SetVisibility(bool new_visibility)
{
    if (visible_ == new_visibility) return;

    RenderQueue* queue = PlanetEngine::Get()->GetRenderQueue();

    visible_ = new_visibility;
    if (visible_)
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
