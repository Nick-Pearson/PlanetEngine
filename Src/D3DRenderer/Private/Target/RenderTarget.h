#pragma once

#include <d3d12.h>

struct D3DResource
{
    ID3D12Resource* resource_ = nullptr;
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle_;
};

class RenderTarget
{
 public:
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual const D3DResource* GetRenderTarget() const = 0;
    virtual const D3DResource* GetDepthStencil() const = 0;
    virtual ID3D12CommandAllocator* GetCommandAllocator() const = 0;
};
