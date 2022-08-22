#include "D3DTextureLoader.h"

#include "d3dx12.h"

#include "PlanetLogging.h"
#include "Texture/Texture2D.h"
#include "Texture/ComputeTexture2D.h"
#include "Texture/ComputeTexture3D.h"
#include "D3DAssert.h"

D3DTextureLoader::D3DTextureLoader(ID3D12GraphicsCommandList* command_list, ID3D12Device2* device) :
    command_list_(command_list), device_(device)
{
    command_list_->AddRef();
    device_->AddRef();
}

D3DTextureLoader::~D3DTextureLoader()
{
    command_list_->Release();
    device_->Release();
}

D3DTexture* D3DTextureLoader::Load(const Texture* texture)
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
    return nullptr;
}


// ID3D11UnorderedAccessView* D3DTextureLoader::LoadForCompute(const Texture* texture)
// {
    // LoadedTexture texture_resource = GetOrLoadTexture(texture);

    // D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    // uavDesc.Format = texture_resource.format_;

    // auto dimensions = texture->GetDimensions();
    // if (dimensions == TextureDimensions::_1D)
    // {
    //     uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1D;
    // }
    // else if (dimensions == TextureDimensions::_2D)
    // {
    //     uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
    // }
    // else if (dimensions == TextureDimensions::_3D)
    // {
    //     uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
    //     uavDesc.Texture3D.MipSlice = 0;
    //     uavDesc.Texture3D.FirstWSlice = 0;
    //     uavDesc.Texture3D.WSize = -1;
    // }

    // ID3D11UnorderedAccessView* uav_view = nullptr;
    // d3dAssert(device_->CreateUnorderedAccessView(texture_resource.resource_, &uavDesc, &uav_view));
//     return nullptr;
// }

D3DTexture* D3DTextureLoader::LoadTexture2D(const class Texture2D* texture)
{
    D3D12_RESOURCE_DESC resource_desc{};
    resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resource_desc.Alignment = 0;
    resource_desc.Width = texture->GetWidth();
    resource_desc.Height = texture->GetHeight();
    resource_desc.DepthOrArraySize = 1;
    resource_desc.MipLevels = 1;
    resource_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    // multisampling
    resource_desc.SampleDesc.Count = 1;
    resource_desc.SampleDesc.Quality = 0;
    resource_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    ID3D12Resource* resource;
    d3dAssert(device_->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &resource_desc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&resource)));
    SET_NAME(resource, L"Texture Buffer Resource Heap");

    const auto stride = texture->GetWidth() * sizeof(Colour);
    const auto buffer_size = texture->GetHeight() * stride;
    ID3D12Resource* intermediate_resource;
    d3dAssert(device_->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(buffer_size),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&intermediate_resource)));

    D3D12_SUBRESOURCE_DATA subresource_data = {};
    subresource_data.pData = texture->GetData();
    subresource_data.RowPitch = texture->GetHeight();
    subresource_data.SlicePitch = stride;
    UpdateSubresources(command_list_, resource, intermediate_resource, 0, 0, 1, &subresource_data);

    // CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource,
    //         D3D12_RESOURCE_STATE_COPY_DEST,
    //         D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    // command_list_->ResourceBarrier(1, &barrier);

    return new D3DTexture{resource,
        intermediate_resource,
        resource_desc.Format};
}

D3DTexture* D3DTextureLoader::LoadComputeTexture2D(const ComputeTexture2D* texture)
{
    // create texture resource
    // D3D11_TEXTURE2D_DESC textureDesc = {};
    // textureDesc.Width = texture->GetWidth();
    // textureDesc.Height = texture->GetHeight();
    // textureDesc.MipLevels = 1;
    // textureDesc.ArraySize = 1;
    // textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    // textureDesc.SampleDesc.Count = 1;
    // textureDesc.SampleDesc.Quality = 0;
    // textureDesc.Usage = D3D11_USAGE_DEFAULT;
    // textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    // textureDesc.CPUAccessFlags = 0;
    // textureDesc.MiscFlags = 0;
    // ID3D11Texture2D* d3d11Texture;
    // d3dAssert(device_->CreateTexture2D(&textureDesc, NULL, &d3d11Texture));
    // return LoadedTexture{nullptr, nullptr, DXGI_FORMAT_R32G32B32A32_FLOAT};
    return nullptr;
}

D3DTexture* D3DTextureLoader::LoadComputeTexture3D(const ComputeTexture3D* texture)
{
    // create texture resource
    // D3D11_TEXTURE3D_DESC textureDesc = {};
    // textureDesc.Width = texture->GetWidth();
    // textureDesc.Height = texture->GetHeight();
    // textureDesc.Depth = texture->GetDepth();
    // textureDesc.MipLevels = 1;
    // textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    // textureDesc.Usage = D3D11_USAGE_DEFAULT;
    // textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    // textureDesc.CPUAccessFlags = 0;
    // textureDesc.MiscFlags = 0;
    // ID3D11Texture3D* d3d11Texture;
    // d3dAssert(device_->CreateTexture3D(&textureDesc, NULL, &d3d11Texture));
    // return LoadedTexture{nullptr, nullptr, DXGI_FORMAT_R8G8B8A8_UNORM };
    return nullptr;
}