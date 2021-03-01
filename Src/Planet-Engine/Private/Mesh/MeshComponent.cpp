#include "MeshComponent.h"

#include "PlanetEngine.h"
#include "Render/RenderSystem.h"
#include "Render/Renderer.h"
#include "Render/ResourceManager.h"
#include "Mesh.h"
#include "Entity/Entity.h"
#include "Material/Material.h"

MeshComponent::MeshComponent(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) :
    mMesh(mesh), mMaterial(material)
{
}

void MeshComponent::SetVisibility(bool newVisibility)
{
    if (mVisible == newVisibility) return;

    Renderer* renderer = PlanetEngine::Get()->GetRenderSystem()->GetRenderer();

    mVisible = newVisibility;
    if (mVisible)
    {
        renderState.debugName = GetParent()->GetName();
        renderStatePtr = renderer->AddRenderState(renderState);
    }
    else
    {
        renderer->RemoveRenderState(renderStatePtr);
        renderStatePtr = nullptr;
    }
}

void MeshComponent::SetUseDepthBuffer(bool useDepthBuffer)
{
    if (renderStatePtr)
        renderStatePtr->UseDepthBuffer = useDepthBuffer;

    renderState.UseDepthBuffer = useDepthBuffer;
}

void MeshComponent::SetUseWorldMatrix(bool useWorldMatrix)
{
    if (renderStatePtr)
        renderStatePtr->UseWorldMatrix = useWorldMatrix;

    renderState.UseWorldMatrix = useWorldMatrix;
}

void MeshComponent::OnSpawned()
{
    Component::OnSpawned();

    RenderSystem* renderSystem = PlanetEngine::Get()->GetRenderSystem();

    renderState.mesh = renderSystem->GetResourceManager()->LoadMesh(mMesh);
    renderState.material = renderSystem->GetResourceManager()->LoadMaterial(mMaterial);
    renderState.model = GetWorldTransform();

    SetVisibility(true);
}

void MeshComponent::OnDestroyed()
{
    Component::OnDestroyed();

    SetVisibility(false);
}

void MeshComponent::OnEntityTransformChanged()
{
    Transform worldTransform = GetWorldTransform();

    if (renderStatePtr)
        renderStatePtr->model = worldTransform;

    renderState.model = worldTransform;
}
