#pragma once

#include <d3d12.h>

#include "Texture/Texture.h"
#include "TextureResource.h"

class D3DTexture : public TextureResource
{
 public:
    D3DTexture(ID3D12Device2* device, const Texture* texture);

    virtual ~D3DTexture();

    inline bool IsLoaded() const override { return loaded_; }

    void OnLoadingComplete();

    void TransitionResource(ID3D12GraphicsCommandList* command_list) const;

    ID3D12Resource* const resource_;
    DXGI_FORMAT const format_;
    int const dimensions_;

 private:
    bool loaded_ = false;
    const D3D12_RESOURCE_STATES initial_state_;

    ID3D12Resource* const intermediate_resource_;
};