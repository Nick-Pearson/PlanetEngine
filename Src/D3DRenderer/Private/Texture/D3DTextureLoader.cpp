#include "D3DTextureLoader.h"

#include "PlanetLogging.h"
#include "Texture/Texture2D.h"
#include "Texture/ComputeTexture2D.h"
#include "D3DAssert.h"

D3DTextureLoader::D3DTextureLoader(wrl::ComPtr<ID3D11Device> device,
    wrl::ComPtr<ID3D11DeviceContext> context,
    D3DShaderLoader* shader_loader) :
    device_(device), context_(context), shader_loader_(shader_loader)
{
}


std::shared_ptr<D3DTexture> D3DTextureLoader::Load(const Texture* texture)
{
    wrl::ComPtr<ID3D11ShaderResourceView> texture_view = LoadTextureView(texture);

    if (!texture_view)
    {
        return nullptr;
    }

    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    wrl::ComPtr<ID3D11SamplerState> sampler_state;
    d3dAssert(device_->CreateSamplerState(&samplerDesc, &sampler_state));

    return std::make_shared<D3DTexture>(texture_view, sampler_state);
}

wrl::ComPtr<ID3D11ShaderResourceView> D3DTextureLoader::LoadTextureView(const Texture* texture)
{
    TextureDimensions dimensions = texture->GetDimensions();
    TextureDataType data_type = texture->GetDataType();

    if (data_type == TextureDataType::STATIC && dimensions == TextureDimensions::_2D)
    {
        return LoadTexture2DView(static_cast<const Texture2D*>(texture));
    }
    else if (data_type == TextureDataType::COMPUTE && dimensions == TextureDimensions::_2D)
    {
        return ComputeTexture2DView(static_cast<const ComputeTexture2D*>(texture));
    }


    P_ERROR("unsupported texture data type {} {}", data_type, dimensions);
    return nullptr;
}

wrl::ComPtr<ID3D11ShaderResourceView> D3DTextureLoader::LoadTexture2DView(const Texture2D* texture)
{
    // create texture resource
    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = texture->GetWidth();
    textureDesc.Height = texture->GetHeight();
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = texture->GetData();
    sd.SysMemPitch = texture->GetWidth() * sizeof(Colour);
    wrl::ComPtr<ID3D11Texture2D> d3d11Texture;
    d3dAssert(device_->CreateTexture2D(&textureDesc, &sd, &d3d11Texture));

    // create the resource view on the texture
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    wrl::ComPtr<ID3D11ShaderResourceView> texture_view;
    d3dAssert(device_->CreateShaderResourceView(d3d11Texture.Get(), &srvDesc, &texture_view));
    return texture_view;
}

wrl::ComPtr<ID3D11ShaderResourceView> D3DTextureLoader::ComputeTexture2DView(const class ComputeTexture2D* texture)
{
    // create texture resource
    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = texture->GetWidth();
    textureDesc.Height = texture->GetHeight();
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;
    wrl::ComPtr<ID3D11Texture2D> d3d11Texture;
    d3dAssert(device_->CreateTexture2D(&textureDesc, NULL, &d3d11Texture));

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.Format = textureDesc.Format;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
    uavDesc.Texture2D.MipSlice = 0;
    ID3D11UnorderedAccessView* uav_view;
    d3dAssert(device_->CreateUnorderedAccessView(d3d11Texture.Get(), &uavDesc, &uav_view));

    // invoke the compute shader to populate the texture
    auto shader = shader_loader_->LoadCompute(texture->GetShaderName().c_str());
    shader->Use(context_.Get());
    context_->CSSetUnorderedAccessViews(0u, 1u, &uav_view, nullptr);
    shader->Invoke(context_.Get());
    uav_view->Release();
    ID3D11UnorderedAccessView* null_uav = nullptr;
    context_->CSSetUnorderedAccessViews(0u, 1u, &null_uav, nullptr);

    // create the resource view on the texture for use in shaders
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    wrl::ComPtr<ID3D11ShaderResourceView> shader_view;
    d3dAssert(device_->CreateShaderResourceView(d3d11Texture.Get(), &srvDesc, &shader_view));
    return shader_view;
}