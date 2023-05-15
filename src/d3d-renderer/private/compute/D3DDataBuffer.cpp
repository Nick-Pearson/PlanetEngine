#include "D3DDataBuffer.h"

#include "d3dx12.h"

#include "D3DAssert.h"
#include "compute/ComputeShader.h"

D3DDataBuffer::D3DDataBuffer(ID3D12Device2* device, const DataBuffer* buffer) :
    device_(device), count_(buffer->count_), stride_(buffer->stride_), cpu_data_(buffer->data_)
{
    device_->AddRef();

    // strucutred buffer
    D3D12_RESOURCE_DESC resource_desc{};
    resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resource_desc.Alignment = 0;
    resource_desc.MipLevels = 1;
    resource_desc.Width = buffer->stride_ * buffer->count_;
    resource_desc.Height = 1;
    resource_desc.DepthOrArraySize = 1;
    resource_desc.Format = DXGI_FORMAT_UNKNOWN;
    resource_desc.SampleDesc.Count = 1;
    resource_desc.SampleDesc.Quality = 0;
    resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    d3dAssert(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &resource_desc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&resource_)));
    SET_NAME(resource_, "Buffer");

    uint32_t num_rows;
    uint64_t row_size;
    device_->GetCopyableFootprints(&resource_desc, 0U, 1U, 0U, nullptr, &num_rows, &row_size, &total_size_);
}

D3DDataBuffer::~D3DDataBuffer()
{
    device_->Release();
}

void D3DDataBuffer::Copy(ID3D12GraphicsCommandList* command_list) const
{
    ID3D12Resource* intermediate_resource;
    d3dAssert(device_->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(total_size_),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&intermediate_resource)));
    SET_NAME(intermediate_resource, "Buffer Intermediate");

    D3D12_SUBRESOURCE_DATA subresource_data = {};
    subresource_data.pData = cpu_data_;
    subresource_data.RowPitch = stride_;
    subresource_data.SlicePitch = count_ * stride_;
    UpdateSubresources(command_list, resource_, intermediate_resource, 0, 0, 1, &subresource_data);
}