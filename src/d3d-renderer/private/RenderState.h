#pragma once

#include "Mesh/D3DMesh.h"
#include "Material/D3DMaterial.h"
#include "D3DRootSignature.h"
#include "D3DPipelineState.h"

#include "Math/Transform.h"

struct RenderState
{
 public:
    RenderState(D3DMesh* mesh,
                D3DMaterial* material,
                D3DRootSignature* root_signature,
                D3DPipelineState* pipeline_state,
                const Transform& model,
                bool use_depth_buffer,
                bool use_world_matrix) :
        mesh_(mesh), material_(material), root_signature_(root_signature),
        pipeline_state_(pipeline_state), model_(model),
        use_depth_buffer_(use_depth_buffer), use_world_matrix_(use_world_matrix)
    {
    }

    bool IsValid() const
    {
        return mesh_->IsLoaded() && material_->IsLoaded();
    }

    D3DMesh* const mesh_;
    D3DMaterial* const material_;
    D3DRootSignature* const root_signature_;
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
            root_signature_ == other.root_signature_ &&
            pipeline_state_ == other.pipeline_state_ &&
            model_ == other.model_;
    }
};