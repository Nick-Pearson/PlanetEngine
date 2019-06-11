#pragma once

#include <memory>

#include "../Entity/Component.h"
#include "../Renderer/RenderState.h"

class Mesh;

class MeshComponent : public Component
{
public:
	MeshComponent(std::shared_ptr<Mesh> mesh, const char* shaderName);

	void SetVisibility(bool newVisibility);

	void SetUseDepthBuffer(bool useDepthBuffer);
	void SetUseWorldMatrix(bool useWorldMatrix);

	void OnSpawned() override;
	void OnDestroyed() override;

private:

	bool mVisible = false;

	RenderState renderState;
	RenderState* renderStatePtr = nullptr;
};