#pragma once

#include <memory>

#include "Entity/Component.h"

class Mesh;
class Material;


struct RenderConfig
{
    bool use_depth_buffer_ = true;
    bool use_world_matrix_ = true;
};
class MeshComponent : public Component
{
 public:
    MeshComponent(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);

    void SetVisibility(bool new_visibility);

    void OnSpawned() override;
    void OnDestroyed() override;

    inline const Mesh* GetMesh() const { return mesh_.get(); }
    inline const Material* GetMaterial() const { return material_.get(); }

    RenderConfig render_config_;
 private:
    bool visible_ = false;
    std::shared_ptr<Mesh> mesh_;
    std::shared_ptr<Material> material_;
};