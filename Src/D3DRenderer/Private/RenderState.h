#pragma once

#include "Mesh/D3DMesh.h"
#include "Material/D3DMaterial.h"

#include "Math/Transform.h"

struct RenderState
{
 public:
    RenderState(D3DMesh* mesh,
                D3DMaterial* material,
                const Transform& model,
                bool use_depth_buffer,
                bool use_world_matrix) :
        mesh_(mesh), material_(material), model_(model),
        use_depth_buffer_(use_depth_buffer), use_world_matrix_(use_world_matrix)
    {
    }

    bool IsValid() const
    {
        return mesh_->IsLoaded() && material_->IsLoaded();
    }

    D3DMesh* const mesh_;
    D3DMaterial* const material_;
    Transform model_;
    bool use_depth_buffer_;
    bool use_world_matrix_;

    bool operator==(const RenderState& other) const
    {
        return use_depth_buffer_ == other.use_depth_buffer_ &&
            use_world_matrix_ == other.use_world_matrix_ &&
            mesh_ == other.mesh_ &&
            material_ == other.material_ &&
            model_ == other.model_;
    }
};