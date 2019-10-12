#pragma once

#include <memory>

#include "../Math/Transform.h"

class D3DShader;
struct GPUMeshHandle;

struct RenderState
{
public:

	RenderState() :
		UseDepthBuffer(true), UseWorldMatrix(true), mesh(nullptr), pixelShader(nullptr)
	{
	}

	RenderState(GPUMeshHandle* inMesh, std::shared_ptr<D3DShader> inShader) :
		UseDepthBuffer(true), UseWorldMatrix(true), mesh(inMesh), pixelShader(inShader)
	{
	}

	bool IsValid() const 
	{
		return mesh && pixelShader;
	}

	bool UseDepthBuffer;
	bool UseWorldMatrix;

	GPUMeshHandle* mesh;
	Transform model;
	const char* debugName;

	// vertex shader?
	std::shared_ptr<D3DShader> pixelShader;
};