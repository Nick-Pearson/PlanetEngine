#include "D3DTexture.h"

#include "d3dx12.h"

D3DTexture::D3DTexture(
            ID3D12Resource* resource,
            D3D12_RESOURCE_STATES initial_state,
            ID3D12Resource* intermediate_resource,
            DXGI_FORMAT format,
            int dimensions) :
    resource_(resource),
    initial_state_(initial_state),
    intermediate_resource_(intermediate_resource),
    format_(format),
    dimensions_(dimensions)
{
}

D3DTexture::~D3DTexture()
{
    resource_->Release();
    intermediate_resource_->Release();
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