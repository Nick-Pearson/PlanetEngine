#pragma once

#include <memory>

#include "Mesh/D3DMesh.h"

#include "Math/Transform.h"

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

    MeshResource* mesh;
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