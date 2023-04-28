#include "WindowRenderTarget.h"

#include "d3dx12.h"

WindowRenderTarget::WindowRenderTarget(ID3D12Device2* device,
        IDXGISwapChain4* swap_chain,
        ID3D12DescriptorHeap* rtv_heap,
        ID3D12DescriptorHeap* dsv_heap,
        D3DCommandQueue* command_queue) :
    swap_chain_(swap_chain), rtv_heap_(rtv_heap), dsv_heap_(dsv_heap), command_queue_(command_queue)
{
    swap_chain_->AddRef();
    rtv_heap_->AddRef();
    dsv_heap_->AddRef();

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
    auto current_buffer = swap_chain_->GetCurrentBackBufferIndex();
    command_queue_->WaitForSignal(frame_signals_[current_buffer]);

    swap_chain_->Release();
    rtv_heap_->Release();
    dsv_heap_->Release();

    depth_stencil_view_.resource_->Release();
    for (int i = 0; i < NUM_BUFFERS; ++i)
    {
        target_view_[i].resource_->Release();
        command_allocator_[i]->Release();
    }
}

void WindowRenderTarget::PreRender()
{
    current_buffer_ = swap_chain_->GetCurrentBackBufferIndex();
    command_queue_->WaitForSignal(frame_signals_[current_buffer_]);
}

void WindowRenderTarget::Present()
{
    UINT syncInterval = 1;
    UINT presentFlags = 0;
    d3dAssert(swap_chain_->Present(syncInterval, presentFlags));

    frame_signals_[current_buffer_] = command_queue_->Signal();
}

void WindowRenderTarget::UpdateWindowSize(ID3D12Device* device)
{
    for (int i = 0; i < NUM_BUFFERS; ++i)
    {
        command_queue_->WaitForSignal(frame_signals_[i]);

        if (target_view_[i].resource_)
            target_view_[i].resource_->Release();

        target_view_[i].resource_ = nullptr;
    }

    d3dAssert(swap_chain_->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0));
    DXGI_SWAP_CHAIN_DESC swap_chain_desc;
    d3dAssert(swap_chain_->GetDesc(&swap_chain_desc));
    width_ = swap_chain_desc.BufferDesc.Width;
    height_ = swap_chain_desc.BufferDesc.Height;

    auto rtv_descriptor_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle{rtv_heap_->GetCPUDescriptorHandleForHeapStart()};

    for (int i = 0; i < NUM_BUFFERS; ++i)
    {
        ID3D12Resource* back_buffer;
        d3dAssert(swap_chain_->GetBuffer(i, IID_PPV_ARGS(&back_buffer)));

        device->CreateRenderTargetView(back_buffer, nullptr, rtv_handle);

        target_view_[i].resource_ = back_buffer;
        target_view_[i].cpu_handle_ = rtv_handle;

        rtv_handle.Offset(rtv_descriptor_size);
    }

    D3D12_CLEAR_VALUE clear_value = {};
    clear_value.Format = DXGI_FORMAT_D32_FLOAT;
    clear_value.DepthStencil = { 1.0f, 0 };

    // Update the depth-stencil view.
    CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT,
        width_, height_,
        1, 0, 1, 0,
        D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE);

    ID3D12Resource* depth_buffer;
    d3dAssert(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clear_value,
        IID_PPV_ARGS(&depth_buffer)));

    D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
    dsv.Format = DXGI_FORMAT_D32_FLOAT;
    dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsv.Texture2D.MipSlice = 0;
    dsv.Flags = D3D12_DSV_FLAG_NONE;
    device->CreateDepthStencilView(depth_buffer, &dsv, dsv_heap_->GetCPUDescriptorHandleForHeapStart());

    depth_stencil_view_.resource_ = depth_buffer;
    depth_stencil_view_.cpu_handle_ = dsv_heap_->GetCPUDescriptorHandleForHeapStart();

    current_buffer_ = swap_chain_->GetCurrentBackBufferIndex();
}