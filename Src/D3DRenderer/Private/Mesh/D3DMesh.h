#pragma once

#include <d3d12.h>

#include "MeshResource.h"

#include "Mesh/Mesh.h"

class D3DMesh : public MeshResource
{
 public:
    D3DMesh(const Mesh* mesh,
            ID3D12Resource* vertex_buffer,
            ID3D12Resource* vertex_intermediate_buffer,
            ID3D12Resource* triangle_buffer,
            ID3D12Resource* triangle_intermediate_buffer);
    virtual ~D3DMesh();

    inline const D3D12_VERTEX_BUFFER_VIEW* GetVertexBuffer() const override { return &vertex_buffer_view_; }
    inline const D3D12_INDEX_BUFFER_VIEW* GetTriangleBuffer() const override { return &triangle_buffer_view_; }
    inline size_t GetTriangleCount() const override { return mesh_->GetTriangleCount(); }
    inline bool IsLoaded() const override { return loaded_; }

    void OnLoadingComplete();

 private:
    const Mesh* mesh_;

    bool loaded_ = false;

    ID3D12Resource* vertex_buffer_;
    ID3D12Resource* vertex_intermediate_buffer_;
    D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view_;

    ID3D12Resource* triangle_buffer_;
    ID3D12Resource* triangle_intermediate_buffer_;
    D3D12_INDEX_BUFFER_VIEW triangle_buffer_view_;
};