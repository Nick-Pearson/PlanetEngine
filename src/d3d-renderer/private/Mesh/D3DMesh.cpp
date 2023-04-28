#include "D3DMesh.h"

D3DMesh::D3DMesh(const Mesh* mesh,
        ID3D12Resource* vertex_buffer, ID3D12Resource* vertex_intermediate_buffer,
        ID3D12Resource* triangle_buffer, ID3D12Resource* triangle_intermediate_buffer) :
    mesh_(mesh),
    vertex_buffer_(vertex_buffer), vertex_intermediate_buffer_(vertex_intermediate_buffer),
    triangle_buffer_(triangle_buffer), triangle_intermediate_buffer_(triangle_intermediate_buffer)
{
    vertex_buffer_->AddRef();
    vertex_intermediate_buffer_->AddRef();
    triangle_buffer_->AddRef();
    triangle_intermediate_buffer_->AddRef();

    vertex_buffer_view_ = D3D12_VERTEX_BUFFER_VIEW{};
    vertex_buffer_view_.BufferLocation = vertex_buffer_->GetGPUVirtualAddress();
    vertex_buffer_view_.SizeInBytes = sizeof(Vertex) * mesh->GetVertexCount();
    vertex_buffer_view_.StrideInBytes = sizeof(Vertex);

    triangle_buffer_view_.BufferLocation = triangle_buffer_->GetGPUVirtualAddress();
    triangle_buffer_view_.Format = DXGI_FORMAT_R16_UINT;
    triangle_buffer_view_.SizeInBytes = sizeof(uint16_t) * mesh->GetTriangleCount();
}

D3DMesh::~D3DMesh()
{
    vertex_buffer_->Release();
    vertex_buffer_ = nullptr;
    vertex_intermediate_buffer_->Release();
    vertex_intermediate_buffer_ = nullptr;
    triangle_buffer_->Release();
    triangle_buffer_ = nullptr;
    triangle_intermediate_buffer_->Release();
    triangle_intermediate_buffer_ = nullptr;
}

void D3DMesh::OnLoadingComplete()
{
    loaded_ = true;
}