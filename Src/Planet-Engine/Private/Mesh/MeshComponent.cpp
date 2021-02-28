#include "MeshComponent.h"

#include "../Renderer/Renderer.h"
#include "PlanetEngine.h"
#include "GPUResourceManager.h"
#include "../Renderer/D3D11/D3DShader.h"
#include "Mesh.h"
#include "../Entity/Entity.h"
#include "../Renderer/RenderManager.h"
#include "../Material/Material.h"

MeshComponent::MeshComponent(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) :
    mMesh(mesh), mMaterial(material)
{
}

void MeshComponent::SetVisibility(bool newVisibility)
{
    if (mVisible == newVisibility) return;

    Renderer* renderer = PlanetEngine::Get()->GetRenderManager()->GetRenderer();

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

    Renderer* renderer = PlanetEngine::Get()->GetRenderManager()->GetRenderer();

    renderState.mesh = renderer->GetResourceManager()->LoadMesh(mMesh);
    renderState.material = renderer->GetResourceManager()->LoadMaterial(mMaterial);
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
