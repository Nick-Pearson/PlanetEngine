#pragma once

#include <d3d11.h>

class D3DTexture
{
 public:
    explicit D3DTexture(ID3D11Resource* texture_resource, ID3D11ShaderResourceView* texture_view, ID3D11SamplerState* sampler_state);
    ~D3DTexture();

    void Use(struct ID3D11DeviceContext* context, int slot);

 private:
    ID3D11Resource* texture_resource_;
    ID3D11SamplerState* sampler_state_;
    ID3D11ShaderResourceView* texture_view_;
};