#pragma once

#include <d3d12.h>

#include "Mesh/Mesh.h"

class D3DMesh
{
 public:
    D3DMesh(const Mesh* mesh,
            ID3D12Resource* vertex_buffer,
            ID3D12Resource* vertex_intermediate_buffer,
            ID3D12Resource* triangle_buffer,
            ID3D12Resource* triangle_intermediate_buffer);
    virtual ~D3DMesh();

    inline bool IsLoaded() const { return loaded_; }

    void OnLoadingComplete();
    void Draw(ID3D12GraphicsCommandList* command_list);

 private:
    const Mesh* mesh_;

    bool loaded_ = false;
    bool first_bind_ = true;

    ID3D12Resource* vertex_buffer_;
    ID3D12Resource* vertex_intermediate_buffer_;
    D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view_;

    ID3D12Resource* triangle_buffer_;
    ID3D12Resource* triangle_intermediate_buffer_;
    D3D12_INDEX_BUFFER_VIEW triangle_buffer_view_;
};