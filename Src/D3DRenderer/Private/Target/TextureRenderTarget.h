#pragma once

#include <d3d11.h>

#include "Texture/Texture2D.h"

#include "RenderTarget.h"

class TextureRenderTarget : public RenderTarget
{
 public:
    TextureRenderTarget(ID3D11Device* device, int width, int height);
    ~TextureRenderTarget();

    void CopyToTexture(ID3D11Device* device, ID3D11DeviceContext* context, Texture2D* texture);

    inline int GetWidth() const override { return width_; }
    inline int GetHeight() const override { return height_; }
    inline ID3D11RenderTargetView* GetRenderTarget() const override { return target_view_; }
    inline ID3D11DepthStencilView* GetDepthStencil() const override { return depth_stencil_view_; }

 private:
    int width_;
    int height_;
    ID3D11Texture2D* target_texture_ = nullptr;
    ID3D11RenderTargetView* target_view_ = nullptr;
    ID3D11DepthStencilView* depth_stencil_view_ = nullptr;
};
