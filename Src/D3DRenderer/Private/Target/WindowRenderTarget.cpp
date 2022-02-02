#include "WindowRenderTarget.h"

#include "d3dx12.h"

#include "../D3DAssert.h"
#include "../D3DFence.h"

WindowRenderTarget::WindowRenderTarget(ID3D12Device2* device,
        IDXGISwapChain4* swap_chain,
        ID3D12DescriptorHeap* rtv_heap,
        ID3D12CommandQueue* command_queue) :
    swap_chain_(swap_chain), rtv_heap_(rtv_heap)
{
    swap_chain_->AddRef();
    rtv_heap_->AddRef();
    frame_fence_ = new D3DFence{device, command_queue};

    for (int i = 0; i < NUM_BUFFERS; ++i)
    {
        d3dAssert(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&command_allocator_[i])));
        SET_NAME_F(command_allocator_[i], "Window Allocator %d", i)

        frame_signals_[i] = 0;
    }

    UpdateWindowSize(device);
}

WindowRenderTarget::~WindowRenderTarget()
{
    swap_chain_->Release();
    rtv_heap_->Release();

    for (int i = 0; i < NUM_BUFFERS; ++i)
    {
        target_view_[i].resource_->Release();
        depth_stencil_view_[i].resource_->Release();
        command_allocator_[i]->Release();
    }

    delete frame_fence_;
}


void WindowRenderTarget::PreRender()
{
    current_buffer_ = swap_chain_->GetCurrentBackBufferIndex();
    frame_fence_->WaitForSignal(frame_signals_[current_buffer_]);
}

void WindowRenderTarget::Present()
{
    UINT syncInterval = 0;
    UINT presentFlags = 0;
    d3dAssert(swap_chain_->Present(syncInterval, presentFlags));

    frame_signals_[current_buffer_] = frame_fence_->Signal();
}

void WindowRenderTarget::UpdateWindowSize(ID3D12Device* device)
{
    d3dAssert(swap_chain_->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0));

    auto rtv_descriptor_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle{rtv_heap_->GetCPUDescriptorHandleForHeapStart()};

    // auto dsv_descriptor_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    // CD3DX12_CPU_DESCRIPTOR_HANDLE dsv_handle{dsv_heap_->GetCPUDescriptorHandleForHeapStart()};

    for (int i = 0; i < NUM_BUFFERS; ++i)
    {
        if (target_view_[i].resource_)
            target_view_[i].resource_->Release();

        ID3D12Resource* back_buffer;
        d3dAssert(swap_chain_->GetBuffer(i, IID_PPV_ARGS(&back_buffer)));

        device->CreateRenderTargetView(back_buffer, nullptr, rtv_handle);

        target_view_[i].resource_ = back_buffer;
        target_view_[i].cpu_handle_ = rtv_handle;

        rtv_handle.Offset(rtv_descriptor_size);
    }

    DXGI_SWAP_CHAIN_DESC swap_chain_desc;
    d3dAssert(swap_chain_->GetDesc(&swap_chain_desc));

    width_ = swap_chain_desc.BufferDesc.Width;
    height_ = swap_chain_desc.BufferDesc.Height;

    // create depth stencil texture
    // ID3D11Texture2D* depth_stencil = nullptr;
    // D3D11_TEXTURE2D_DESC descDepth = {};
    // descDepth.Width = (UINT)width_;
    // descDepth.Height = (UINT)height_;
    // descDepth.MipLevels = 1u;
    // descDepth.ArraySize = 1u;
    // descDepth.Format = DXGI_FORMAT_D32_FLOAT;
    // descDepth.SampleDesc.Count = 1u;
    // descDepth.SampleDesc.Quality = 0u;
    // descDepth.Usage = D3D11_USAGE_DEFAULT;
    // descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    // d3dAssert(device->CreateTexture2D(&descDepth, nullptr, &depth_stencil));

    // // create view of depth stencil texture
    // D3D12_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    // descDSV.Format = DXGI_FORMAT_D32_FLOAT;
    // descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    // descDSV.Texture2D.MipSlice = 0u;
    // d3dAssert(device->CreateDepthStencilView(depth_stencil, &descDSV, &depth_stencil_view_));
    // depth_stencil->Release();

    current_buffer_ = swap_chain_->GetCurrentBackBufferIndex();
}