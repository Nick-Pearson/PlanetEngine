#include "D3DTextureLoader.h"

#include "PlanetLogging.h"
#include "Texture/Texture2D.h"
#include "Texture/ComputeTexture2D.h"
#include "Texture/ComputeTexture3D.h"
#include "D3DAssert.h"

D3DTextureLoader::D3DTextureLoader(wrl::ComPtr<ID3D11Device> device,
    wrl::ComPtr<ID3D11DeviceContext> context,
    D3DShaderLoader* shader_loader) :
    device_(device), context_(context), shader_loader_(shader_loader)
{
}


std::shared_ptr<D3DTexture> D3DTextureLoader::Load(const Texture* texture)
{
    LoadedTexture texture_resource = GetOrLoadTexture(texture);

    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    ID3D11SamplerState* sampler_state;
    d3dAssert(device_->CreateSamplerState(&samplerDesc, &sampler_state));

    // create the resource view on the texture
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = texture_resource.format_;

    auto dimensions = texture->GetDimensions();
    if (dimensions == TextureDimensions::_1D)
    {
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
        srvDesc.Texture1D.MostDetailedMip = 0;
        srvDesc.Texture1D.MipLevels = 1;
    }
    else if (dimensions == TextureDimensions::_2D)
    {
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = 1;
    }
    else if (dimensions == TextureDimensions::_3D)
    {
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
        srvDesc.Texture3D.MostDetailedMip = 0;
        srvDesc.Texture3D.MipLevels = 1;
    }

    ID3D11ShaderResourceView* texture_view;
    d3dAssert(device_->CreateShaderResourceView(texture_resource.resource_, &srvDesc, &texture_view));

    return std::make_shared<D3DTexture>(texture_resource.resource_, texture_view, sampler_state);
}


ID3D11UnorderedAccessView* D3DTextureLoader::LoadForCompute(const Texture* texture)
{
    LoadedTexture texture_resource = GetOrLoadTexture(texture);

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.Format = texture_resource.format_;

    auto dimensions = texture->GetDimensions();
    if (dimensions == TextureDimensions::_1D)
    {
        uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1D;
    }
    else if (dimensions == TextureDimensions::_2D)
    {
        uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
    }
    else if (dimensions == TextureDimensions::_3D)
    {
        uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
        uavDesc.Texture3D.MipSlice = 0;
        uavDesc.Texture3D.FirstWSlice = 0;
        uavDesc.Texture3D.WSize = -1;
    }

    ID3D11UnorderedAccessView* uav_view = nullptr;
    d3dAssert(device_->CreateUnorderedAccessView(texture_resource.resource_, &uavDesc, &uav_view));
    return uav_view;
}

LoadedTexture D3DTextureLoader::GetOrLoadTexture(const Texture* texture)
{
    const auto it = loaded_textures_.find(texture);
    if (it != loaded_textures_.end())
    {
        it->second.resource_->AddRef();
        return it->second;
    }

    LoadedTexture loaded = LoadTexture(texture);
    loaded_textures_.emplace(texture, loaded);
    return loaded;
}

LoadedTexture D3DTextureLoader::LoadTexture(const Texture* texture)
{
    TextureDimensions dimensions = texture->GetDimensions();
    TextureDataType data_type = texture->GetDataType();

    if (data_type == TextureDataType::STATIC && dimensions == TextureDimensions::_2D)
    {
        return LoadTexture2D(static_cast<const Texture2D*>(texture));
    }
    else if (data_type == TextureDataType::COMPUTE && dimensions == TextureDimensions::_2D)
    {
        return LoadComputeTexture2D(static_cast<const ComputeTexture2D*>(texture));
    }
    else if (data_type == TextureDataType::COMPUTE && dimensions == TextureDimensions::_3D)
    {
        return LoadComputeTexture3D(static_cast<const ComputeTexture3D*>(texture));
    }

    P_FATAL("unsupported texture data type {} {}", data_type, dimensions);
    return LoadedTexture{nullptr, DXGI_FORMAT_UNKNOWN};
}

LoadedTexture D3DTextureLoader::LoadTexture2D(const class Texture2D* texture)
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
    ID3D11Texture2D* d3d11Texture;
    d3dAssert(device_->CreateTexture2D(&textureDesc, &sd, &d3d11Texture));
    return LoadedTexture{d3d11Texture, DXGI_FORMAT_B8G8R8A8_UNORM};
}

LoadedTexture D3DTextureLoader::LoadComputeTexture2D(const ComputeTexture2D* texture)
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
    ID3D11Texture2D* d3d11Texture;
    d3dAssert(device_->CreateTexture2D(&textureDesc, NULL, &d3d11Texture));
    return LoadedTexture{d3d11Texture, DXGI_FORMAT_R32G32B32A32_FLOAT};
}

LoadedTexture D3DTextureLoader::LoadComputeTexture3D(const ComputeTexture3D* texture)
{
    // create texture resource
    D3D11_TEXTURE3D_DESC textureDesc = {};
    textureDesc.Width = texture->GetWidth();
    textureDesc.Height = texture->GetHeight();
    textureDesc.Depth = texture->GetDepth();
    textureDesc.MipLevels = 1;
    textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;
    ID3D11Texture3D* d3d11Texture;
    d3dAssert(device_->CreateTexture3D(&textureDesc, NULL, &d3d11Texture));
    return LoadedTexture{d3d11Texture, DXGI_FORMAT_B8G8R8A8_UNORM };
}