#include "SRVHeap.h"

#include "D3DAssert.h"

namespace
{
    D3D12_SRV_DIMENSION get_srv_dimension(const D3DTexture* texture)
    {
        if (texture->dimensions_ == 1)
        {
            return D3D12_SRV_DIMENSION_TEXTURE1D;
        }
        else if (texture->dimensions_ == 2)
        {
            return D3D12_SRV_DIMENSION_TEXTURE2D;
        }
        else if (texture->dimensions_ == 3)
        {
            return D3D12_SRV_DIMENSION_TEXTURE3D;
        }
        else
        {
            P_FATAL("invalid texture for use in SRV");
        }
    }
}  // namespace

SRVHeap::SRVHeap(ID3D12Device2* device) :
    device_(device)
{
    device_->AddRef();

    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = capacity_;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    d3dAssert(device_->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptor_heap_)));
    cpu_handle_ = descriptor_heap_->GetCPUDescriptorHandleForHeapStart();
    gpu_handle_ = descriptor_heap_->GetGPUDescriptorHandleForHeapStart();
}

SRVHeap::~SRVHeap()
{
    descriptor_heap_->Release();
    device_->Release();
}

D3DDescriptorTable* SRVHeap::CreateDescriptorTable(size_t num_textures, const D3DTexture** textures)
{
    DescriptorHandle root_handle;
    root_handle.cpu_ = cpu_handle_;
    root_handle.gpu_ = gpu_handle_;

    auto descriptor_size = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    for (auto i = 0; i < num_textures; ++i)
    {
        const D3DTexture* texture = textures[i];

        D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
        srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srv_desc.Format = texture->format_;
        srv_desc.ViewDimension = get_srv_dimension(texture);
        srv_desc.Texture2D.MipLevels = 1;

        device_->CreateShaderResourceView(texture->resource_, &srv_desc, cpu_handle_);

        cpu_handle_.ptr += descriptor_size;
        gpu_handle_.ptr += descriptor_size;
    }

    return new D3DDescriptorTable{root_handle, num_textures};
}

void SRVHeap::Bind(ID3D12GraphicsCommandList* command_list)
{
    command_list->SetDescriptorHeaps(1, &descriptor_heap_);
}
