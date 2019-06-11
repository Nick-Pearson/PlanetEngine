#include "MeshComponent.h"

#include "../Renderer/Renderer.h"
#include "../PlanetEngine.h"
#include "GPUResourceManager.h"
#include "../Renderer/D3DShader.h"
#include "Mesh.h"

MeshComponent::MeshComponent(std::shared_ptr<Mesh> mesh, const char* shaderName)
{
	Renderer* renderer = PlanetEngine::Get()->GetRenderer();
	renderer->GetResourceManager()->LoadMesh(mesh);

	renderState.mesh = mesh->GetGPUHandle();
	renderState.pixelShader = renderer->GetResourceManager()->LoadShader(shaderName);
}

void MeshComponent::SetVisibility(bool newVisibility)
{
	if (mVisible == newVisibility) return;

	Renderer* renderer = PlanetEngine::Get()->GetRenderer();

	mVisible = newVisibility;
	if (mVisible)
	{
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

	SetVisibility(true);
}

void MeshComponent::OnDestroyed()
{
	Component::OnDestroyed();

	SetVisibility(false);
}
