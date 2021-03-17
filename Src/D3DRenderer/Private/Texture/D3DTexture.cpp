#include "D3DTexture.h"

D3DTexture::D3DTexture(ID3D11Resource* texture_resource, ID3D11ShaderResourceView* texture_view, ID3D11SamplerState* sampler_state) :
    texture_resource_(texture_resource), texture_view_(texture_view), sampler_state_(sampler_state)
{
}

D3DTexture::~D3DTexture()
{
    texture_resource_->Release();
    texture_view_->Release();
    sampler_state_->Release();
}

void D3DTexture::Use(ID3D11DeviceContext* context, int slot)
{
    context->PSSetShaderResources(slot, 1u, &texture_view_);
    context->PSSetSamplers(slot, 1, &sampler_state_);
}