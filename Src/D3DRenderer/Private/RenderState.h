#pragma once

#include <memory>

#include "Mesh/MeshResource.h"
#include "Material/MaterialResource.h"
#include "D3DPipelineState.h"

#include "Math/Transform.h"

struct RenderState
{
 public:
    RenderState(MeshResource* mesh,
                MaterialResource* material,
                D3DPipelineState* pipeline_state,
                const Transform& model,
                bool use_depth_buffer,
                bool use_world_matrix) :
        mesh_(mesh), material_(material), pipeline_state_(pipeline_state), model_(model),
        use_depth_buffer_(use_depth_buffer), use_world_matrix_(use_world_matrix)
    {
    }

    bool IsValid() const
    {
        return mesh_->IsLoaded() && material_->IsLoaded();
    }

    MeshResource* const mesh_;
    MaterialResource* const material_;
    D3DPipelineState* const pipeline_state_;
    Transform model_;
    bool use_depth_buffer_;
    bool use_world_matrix_;

    bool operator==(const RenderState& other) const
    {
        return use_depth_buffer_ == other.use_depth_buffer_ &&
            use_world_matrix_ == other.use_world_matrix_ &&
            mesh_ == other.mesh_ &&
            material_ == other.material_ &&
            pipeline_state_ == other.pipeline_state_ &&
            model_ == other.model_;
    }
};