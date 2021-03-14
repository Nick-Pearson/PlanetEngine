#include "D3DTexture.h"

#include <d3d11.h>

D3DTexture::D3DTexture(wrl::ComPtr<ID3D11ShaderResourceView> texture_view, wrl::ComPtr<ID3D11SamplerState> sampler_state) :
    texture_view_(texture_view), sampler_state_(sampler_state)
{
}

void D3DTexture::Use(ID3D11DeviceContext* context, int slot)
{
    context->PSSetShaderResources(slot, 1u, texture_view_.GetAddressOf());
    context->PSSetSamplers(slot, 1, sampler_state_.GetAddressOf());
}