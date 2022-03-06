#pragma once

#include <d3d12.h>

#include "D3DRenderSystem.h"
#include "D3DCommandQueue.h"
#include "RenderTarget.h"

class WindowRenderTarget : public RenderTarget
{
 public:
    WindowRenderTarget(ID3D12Device2* device,
      IDXGISwapChain4* swap_chain,
      ID3D12DescriptorHeap* rtv_heap,
      ID3D12DescriptorHeap* dsv_heap,
      D3DCommandQueue* command_queue);
    ~WindowRenderTarget();

    void PreRender();
    void Present();
    void UpdateWindowSize(ID3D12Device* device);

    inline int GetWidth() const override { return width_; }
    inline int GetHeight() const override { return height_; }
    inline const D3DResource* GetRenderTarget() const override { return &target_view_[current_buffer_]; }
    inline const D3DResource* GetDepthStencil() const override { return &depth_stencil_view_; }
    inline ID3D12CommandAllocator* GetCommandAllocator() const override { return command_allocator_[current_buffer_]; }

 private:
    int width_;
    int height_;
    IDXGISwapChain4* swap_chain_;
    ID3D12DescriptorHeap* rtv_heap_;
    ID3D12DescriptorHeap* dsv_heap_;

    UINT current_buffer_ = 0;
    D3DResource target_view_[NUM_BUFFERS];
    D3DResource depth_stencil_view_;
    ID3D12CommandAllocator* command_allocator_[NUM_BUFFERS];

    D3DCommandQueue* command_queue_;
    uint64_t frame_signals_[NUM_BUFFERS];
};
