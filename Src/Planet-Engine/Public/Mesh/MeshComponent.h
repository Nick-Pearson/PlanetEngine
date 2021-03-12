#pragma once

#include <memory>

#include "Entity/Component.h"

class Mesh;
class Material;


struct RenderConfig
{
    bool use_depth_buffer = true;
    bool use_world_matrix = true;
};
class MeshComponent : public Component
{
 public:
    MeshComponent(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);

    void SetVisibility(bool newVisibility);

    void OnSpawned() override;
    void OnDestroyed() override;

    inline const Mesh* GetMesh() const { return mMesh.get(); }
    inline const Material* GetMaterial() const { return mMaterial.get(); }

    RenderConfig render_config_;
 private:
    bool mVisible = false;
    std::shared_ptr<Mesh> mMesh;
    std::shared_ptr<Material> mMaterial;
};