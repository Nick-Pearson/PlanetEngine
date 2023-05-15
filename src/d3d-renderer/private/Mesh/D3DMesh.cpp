#include "D3DMesh.h"

#include "d3dx12.h"

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

void D3DMesh::Draw(ID3D12GraphicsCommandList* command_list)
{
    if (first_bind_)
    {
        first_bind_ = false;

        CD3DX12_RESOURCE_BARRIER b[2];
        b[0] = CD3DX12_RESOURCE_BARRIER::Transition(vertex_buffer_, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        b[1] = CD3DX12_RESOURCE_BARRIER::Transition(triangle_buffer_, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
        command_list->ResourceBarrier(2, b);
    }

    command_list->IASetVertexBuffers(0u, 1u, &vertex_buffer_view_);
    command_list->IASetIndexBuffer(&triangle_buffer_view_);
    const auto instance_count = 1u;
    const auto start_index = 0u;
    const auto start_vertex = 0u;
    const auto start_instance = 0u;
    command_list->DrawIndexedInstanced(mesh_->GetTriangleCount(), instance_count, start_index, start_vertex, start_instance);
}