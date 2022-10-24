#pragma once

#include <d3d12.h>

#include "TextureResource.h"

class D3DTexture : public TextureResource
{
 public:
    explicit D3DTexture(ID3D12Resource* resource, ID3D12Resource* intermediate_resource, DXGI_FORMAT format, int dimensions);
    virtual ~D3DTexture();

    inline bool IsLoaded() const override { return loaded_; }

    void OnLoadingComplete();

    void TransitionResource(ID3D12GraphicsCommandList* command_list) const;

    ID3D12Resource* const resource_;
    DXGI_FORMAT const format_;
    int const dimensions_;

 private:
    bool loaded_ = false;

    ID3D12Resource* const intermediate_resource_;
};