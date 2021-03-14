#pragma once

#include <wrl.h>

class Texture2D;

namespace wrl = Microsoft::WRL;

class D3DTexture
{
 public:
    explicit D3DTexture(wrl::ComPtr<struct ID3D11ShaderResourceView> texture_view, wrl::ComPtr<struct ID3D11SamplerState> sampler_state);

    void Use(struct ID3D11DeviceContext* context, int slot);

 private:
    wrl::ComPtr<struct ID3D11SamplerState> sampler_state_;
    wrl::ComPtr<struct ID3D11ShaderResourceView> texture_view_;
};