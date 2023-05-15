#include "D3DTexture.h"

#include "d3dx12.h"
#include "D3DAssert.h"
#include "PlanetLogging.h"

D3DTexture::D3DTexture(ID3D12Device2* device, const Texture* texture)
{
    P_ASSERT(texture->GetDepth() < 65536, "3D texture depth must fit in 16 bit int");

    D3D12_RESOURCE_DESC resource_desc{};
    resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
    resource_desc.Alignment = 0;
    resource_desc.Width = texture->GetWidth();
    resource_desc.Height = texture->GetHeight();
    resource_desc.DepthOrArraySize = texture->GetDepth();
    
    resource_desc.MipLevels = 1;
    resource_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    // multisampling
    resource_desc.SampleDesc.Count = 1;
    resource_desc.SampleDesc.Quality = 0;
    resource_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

    resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;
    if (texture->gpu_write_)
        resource_desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

    ID3D12Resource* resource;
    d3dAssert(device_->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &resource_desc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(&resource_)));
}

D3DTexture::~D3DTexture()
{
}

void D3DTexture::OnLoadingComplete()
{
    loaded_ = true;
}

void D3DTexture::TransitionResource(ID3D12GraphicsCommandList* command_list) const
{
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource_,
            initial_state_,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    command_list->ResourceBarrier(1, &barrier);
}