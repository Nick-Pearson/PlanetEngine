#pragma once

#include <memory>

#include "Entity/Component.h"
#include "Render/RenderState.h"

class Mesh;
class Material;

class MeshComponent : public Component
{
 public:
    MeshComponent(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);

    void SetVisibility(bool newVisibility);

    void SetUseDepthBuffer(bool useDepthBuffer);
    void SetUseWorldMatrix(bool useWorldMatrix);

    void OnSpawned() override;
    void OnDestroyed() override;

    void OnEntityTransformChanged() override;

 private:
    bool mVisible = false;
    std::shared_ptr<Mesh> mMesh;
    std::shared_ptr<Material> mMaterial;

    RenderState renderState;
    RenderState* renderStatePtr = nullptr;
};