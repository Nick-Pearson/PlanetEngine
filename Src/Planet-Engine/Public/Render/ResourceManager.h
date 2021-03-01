#pragma once

#include <memory>

class Material;
class Mesh;
class GPUMeshHandle;
class GPUMaterialHandle;

class ResourceManager
{
 public:
    virtual GPUMeshHandle* LoadMesh(std::shared_ptr<Mesh> mesh) = 0;
    virtual void UnloadMesh(std::shared_ptr<Mesh> mesh) = 0;

    virtual std::shared_ptr<GPUMaterialHandle> LoadMaterial(std::shared_ptr<Material> material) = 0;
    virtual void ReloadAllShaders() = 0;
};
