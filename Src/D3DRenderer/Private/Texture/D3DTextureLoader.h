#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <memory>

#include "Texture/Texture.h"
#include "Shader/D3DShaderLoader.h"

#include "D3DTexture.h"

namespace wrl = Microsoft::WRL;

class D3DTextureLoader
{
 public:
    explicit D3DTextureLoader(wrl::ComPtr<ID3D11Device> device, wrl::ComPtr<ID3D11DeviceContext> context, D3DShaderLoader* shader_loader);

    std::shared_ptr<D3DTexture> Load(const Texture* texture);

 private:
    wrl::ComPtr<ID3D11ShaderResourceView> LoadTextureView(const Texture* texture);
    wrl::ComPtr<ID3D11ShaderResourceView> LoadTexture2DView(const class Texture2D* texture);
    wrl::ComPtr<ID3D11ShaderResourceView> ComputeTexture2DView(const class ComputeTexture2D* texture);

    wrl::ComPtr<ID3D11Device> device_;
    wrl::ComPtr<ID3D11DeviceContext> context_;
    D3DShaderLoader* shader_loader_;
};