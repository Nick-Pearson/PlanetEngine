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

	void OnEntityTransformChanged() override;

private:

	bool mVisible = false;
	std::shared_ptr<Mesh> mMesh;
	const char* mShaderName;

	RenderState renderState;
	RenderState* renderStatePtr = nullptr;
};