#pragma once

#include <d3d12.h>
#include <unordered_map>
#include <memory>

#include "Texture/Texture.h"

#include "D3DTexture.h"

class D3DTextureLoader
{
 public:
    D3DTextureLoader(ID3D12GraphicsCommandList* copy_command_list, ID3D12Device2* device);
    ~D3DTextureLoader();

    D3DTexture* Load(const Texture* texture);

 private:
    D3DTexture* DoLoadTexture(const Texture* texture);

    D3DTexture* LoadTexture2D(const class Texture2D* texture);
    D3DTexture* LoadComputeTexture2D(const class ComputeTexture2D* texture);
    D3DTexture* LoadComputeTexture3D(const class ComputeTexture3D* texture);

    std::unordered_map<const Texture*, D3DTexture*> loaded_textures_;

    ID3D12Device2* const device_;
    ID3D12GraphicsCommandList* const copy_command_list_;
};