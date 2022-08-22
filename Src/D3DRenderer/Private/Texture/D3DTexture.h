#pragma once

#include <d3d12.h>

#include "TextureResource.h"

class D3DTexture : public TextureResource
{
 public:
    explicit D3DTexture(ID3D12Resource* resource, ID3D12Resource* intermediate_resource, DXGI_FORMAT format);
    virtual ~D3DTexture();

    inline bool IsLoaded() const override { return loaded_; }
    inline D3D12_GPU_VIRTUAL_ADDRESS GetAddr() const { return resource_->GetGPUVirtualAddress(); }

    void OnLoadingComplete();

    ID3D12Resource* const resource_;
    DXGI_FORMAT const format_;

 private:
    bool loaded_ = false;

    ID3D12Resource* const intermediate_resource_;
};