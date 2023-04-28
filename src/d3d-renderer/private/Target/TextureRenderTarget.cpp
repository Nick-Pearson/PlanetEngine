#include "TextureRenderTarget.h"

#include "D3DAssert.h"

TextureRenderTarget::TextureRenderTarget(ID3D12Device2* device, int width, int height)
{
    width_ = width;
    height_ = height;

    // D3D11_TEXTURE2D_DESC texture_desc = {};
    // texture_desc.Width = width_;
    // texture_desc.Height = height_;
    // texture_desc.MipLevels = 1;
    // texture_desc.ArraySize = 1;
    // texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    // texture_desc.SampleDesc.Count = 1;
    // texture_desc.Usage = D3D11_USAGE_DEFAULT;
    // texture_desc.BindFlags = D3D11_BIND_RENDER_TARGET;
    // texture_desc.CPUAccessFlags = 0;
    // texture_desc.MiscFlags = 0;
    // d3dAssert(device->CreateTexture2D(&texture_desc, NULL, &target_texture_));

    // D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc = {};
    // render_target_view_desc.Format = texture_desc.Format;
    // render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    // render_target_view_desc.Texture2D.MipSlice = 0;
    // d3dAssert(device->CreateRenderTargetView(target_texture_, &render_target_view_desc, &target_view_));

    // D3D11_TEXTURE2D_DESC stencil_desc = {};
    // stencil_desc.Width = texture_desc.Width;
    // stencil_desc.Height = texture_desc.Height;
    // stencil_desc.MipLevels = 1u;
    // stencil_desc.ArraySize = 1u;
    // stencil_desc.Format = DXGI_FORMAT_D32_FLOAT;
    // stencil_desc.SampleDesc.Count = 1u;
    // stencil_desc.SampleDesc.Quality = 0u;
    // stencil_desc.Usage = D3D11_USAGE_DEFAULT;
    // stencil_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    // ID3D11Texture2D* depth_stencil_texture = nullptr;
    // d3dAssert(device->CreateTexture2D(&stencil_desc, nullptr, &depth_stencil_texture));

    // D3D11_DEPTH_STENCIL_VIEW_DESC stencil_view_desc = {};
    // stencil_view_desc.Format = DXGI_FORMAT_D32_FLOAT;
    // stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    // stencil_view_desc.Texture2D.MipSlice = 0u;
    // d3dAssert(device->CreateDepthStencilView(depth_stencil_texture, &stencil_view_desc, &depth_stencil_view_));
    // depth_stencil_texture->Release();
    // depth_stencil_texture = nullptr;
}

// void TextureRenderTarget::CopyToTexture(ID3D11Device* device, ID3D11DeviceContext* context, Texture2D* texture)
// {
//     D3D11_TEXTURE2D_DESC staging_desc = {};
//     staging_desc.Width = texture->GetWidth();
//     staging_desc.Height = texture->GetHeight();
//     staging_desc.MipLevels = 1;
//     staging_desc.ArraySize = 1;
//     staging_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//     staging_desc.SampleDesc.Count = 1;
//     staging_desc.Usage = D3D11_USAGE_STAGING;
//     staging_desc.BindFlags = 0;
//     staging_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
//     staging_desc.MiscFlags = 0;
//     ID3D11Texture2D* staging_texture = nullptr;
//     d3dAssert(device->CreateTexture2D(&staging_desc, NULL, &staging_texture));

//     context->CopyResource(staging_texture, target_texture_);

//     D3D11_MAPPED_SUBRESOURCE  mapped_subresource = {};
//     context->Map(staging_texture, 0u, D3D11_MAP_READ, 0u, &mapped_subresource);
//     P_ASSERT(mapped_subresource.RowPitch == sizeof(Colour) * texture->GetWidth(),
//         "Staging texture has invalid RowPitch {}",
//         mapped_subresource.RowPitch);
//     std::memcpy(texture->GetData(), mapped_subresource.pData, mapped_subresource.DepthPitch);
//     context->Unmap(staging_texture, 0u);

//     staging_texture->Release();
// }

TextureRenderTarget::~TextureRenderTarget()
{
    // target_view_->Release();
    // target_view_ = nullptr;

    // depth_stencil_view_->Release();
    // depth_stencil_view_ = nullptr;
}