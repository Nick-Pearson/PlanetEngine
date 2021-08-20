#pragma once

#include <d3d11.h>

class RenderTarget
{
 public:
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual ID3D11RenderTargetView* GetRenderTarget() const = 0;
    virtual ID3D11DepthStencilView* GetDepthStencil() const = 0;
};
