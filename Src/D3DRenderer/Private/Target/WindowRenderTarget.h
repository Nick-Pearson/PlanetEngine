#pragma once

#include <d3d11.h>

#include "RenderTarget.h"

class WindowRenderTarget : public RenderTarget
{
 public:
    WindowRenderTarget(ID3D11Device* device, IDXGISwapChain* swap_chain);
    ~WindowRenderTarget();

    void SwapBuffers();
    void UpdateWindowSize(ID3D11Device* device);

    inline int GetWidth() const override { return width_; }
    inline int GetHeight() const override { return height_; }
    inline ID3D11RenderTargetView* GetRenderTarget() const override { return target_view_; }
    inline ID3D11DepthStencilView* GetDepthStencil() const override { return depth_stencil_view_; }

 private:
    int width_;
    int height_;
    IDXGISwapChain* swap_chain_;
    ID3D11RenderTargetView* target_view_ = nullptr;
    ID3D11DepthStencilView* depth_stencil_view_ = nullptr;
};
