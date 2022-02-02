#pragma once

#include <d3d12.h>

#include "Texture/Texture2D.h"

#include "RenderTarget.h"

class TextureRenderTarget : public RenderTarget
{
 public:
    TextureRenderTarget(ID3D12Device2* device, int width, int height);
    ~TextureRenderTarget();

    //  void CopyToTexture(ID3D12Device* device, ID3D12DeviceContext* context, Texture2D* texture);

    inline int GetWidth() const override { return width_; }
    inline int GetHeight() const override { return height_; }
    inline const D3DResource* GetRenderTarget() const override { return &target_view_; }
    inline const D3DResource* GetDepthStencil() const override { return &depth_stencil_view_; }
    inline ID3D12CommandAllocator* GetCommandAllocator() const override { return nullptr; }

 private:
    int width_;
    int height_;
    ID3D12Resource* target_texture_ = nullptr;
    D3DResource target_view_;
    D3DResource depth_stencil_view_;
};
