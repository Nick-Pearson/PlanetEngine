#pragma once

#include <d3d12.h>
#include <memory>

#include "Texture/Texture.h"

#include "D3DTexture.h"

class D3DTextureLoader
{
 public:
    D3DTextureLoader(ID3D12GraphicsCommandList* command_list, ID3D12Device2* device);
    ~D3DTextureLoader();

    D3DTexture* Load(const Texture* texture);

    // ID3D11UnorderedAccessView* LoadForCompute(const Texture* texture);

 private:
    D3DTexture* LoadTexture2D(const class Texture2D* texture);
    D3DTexture* LoadComputeTexture2D(const class ComputeTexture2D* texture);
    D3DTexture* LoadComputeTexture3D(const class ComputeTexture3D* texture);

    ID3D12Device2* const device_;
    ID3D12GraphicsCommandList* const command_list_;
};