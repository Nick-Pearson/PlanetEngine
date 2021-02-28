#pragma once

#include <memory>

#include "Math/Transform.h"

class D3DShader;
struct GPUMeshHandle;
struct GPUMaterialHandle;

struct RenderState
{
 public:
    RenderState() :
        UseDepthBuffer(true), UseWorldMatrix(true), mesh(nullptr), material(nullptr)
    {
    }

    bool IsValid() const
    {
        return mesh && material;
    }

    bool UseDepthBuffer;
    bool UseWorldMatrix;

    GPUMeshHandle* mesh;
    std::shared_ptr<GPUMaterialHandle> material;
    Transform model;
    const char* debugName;

    bool operator==(const RenderState& other) const
    {
        return UseDepthBuffer == other.UseDepthBuffer &&
            UseWorldMatrix == other.UseWorldMatrix &&
            mesh == other.mesh &&
            material == other.material &&
            model == other.model &&
            debugName == other.debugName;
    }
};