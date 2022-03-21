#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <memory>
#include <unordered_map>

#include "Texture/Texture.h"

#include "D3DTexture.h"

namespace wrl = Microsoft::WRL;

struct LoadedTexture
{
 public:
    LoadedTexture(ID3D11Resource* resource, DXGI_FORMAT format) :
        resource_(resource), format_(format)
    {}

    ID3D11Resource* resource_;
    DXGI_FORMAT format_;
};

class D3DTextureLoader
{
 public:
    explicit D3DTextureLoader(wrl::ComPtr<ID3D11Device> device, wrl::ComPtr<ID3D11DeviceContext> context);

    std::shared_ptr<D3DTexture> Load(const Texture* texture);

    ID3D11UnorderedAccessView* LoadForCompute(const Texture* texture);

 private:
    LoadedTexture GetOrLoadTexture(const Texture* texture);
    LoadedTexture LoadTexture(const Texture* texture);
    LoadedTexture LoadTexture2D(const class Texture2D* texture);
    LoadedTexture LoadComputeTexture2D(const class ComputeTexture2D* texture);
    LoadedTexture LoadComputeTexture3D(const class ComputeTexture3D* texture);

    wrl::ComPtr<ID3D11Device> device_;
    wrl::ComPtr<ID3D11DeviceContext> context_;

    std::unordered_map<const Texture*, LoadedTexture> loaded_textures_;
};