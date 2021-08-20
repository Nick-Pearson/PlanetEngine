#include "WindowRenderTarget.h"

#include "../D3DAssert.h"

WindowRenderTarget::WindowRenderTarget(ID3D11Device* device, IDXGISwapChain* swap_chain)
{
    swap_chain_ = swap_chain;
    UpdateWindowSize(device);
}

WindowRenderTarget::~WindowRenderTarget()
{
    swap_chain_->Release();
    target_view_->Release();
    depth_stencil_view_->Release();
}

void WindowRenderTarget::SwapBuffers()
{
    swap_chain_->Present(1u, 0u);
}

void WindowRenderTarget::UpdateWindowSize(ID3D11Device* device)
{
    if (target_view_)
        target_view_->Release();
    if (depth_stencil_view_)
        depth_stencil_view_->Release();

    d3dAssert(swap_chain_->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0));

    // Get buffer and create a render-target-view.
    ID3D11Texture2D* pBuffer;
    d3dAssert(swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBuffer)));
    d3dAssert(device->CreateRenderTargetView(pBuffer, NULL, &target_view_));
    pBuffer->Release();


    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    d3dAssert(swap_chain_->GetDesc(&swapChainDesc));

    width_ = swapChainDesc.BufferDesc.Width;
    height_ = swapChainDesc.BufferDesc.Height;

    // create depth stencil texture
    ID3D11Texture2D* depth_stencil = nullptr;
    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = (UINT)width_;
    descDepth.Height = (UINT)height_;
    descDepth.MipLevels = 1u;
    descDepth.ArraySize = 1u;
    descDepth.Format = DXGI_FORMAT_D32_FLOAT;
    descDepth.SampleDesc.Count = 1u;
    descDepth.SampleDesc.Quality = 0u;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    d3dAssert(device->CreateTexture2D(&descDepth, nullptr, &depth_stencil));

    // create view of depth stencil texture
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = DXGI_FORMAT_D32_FLOAT;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0u;
    d3dAssert(device->CreateDepthStencilView(depth_stencil, &descDSV, &depth_stencil_view_));
    depth_stencil->Release();
}