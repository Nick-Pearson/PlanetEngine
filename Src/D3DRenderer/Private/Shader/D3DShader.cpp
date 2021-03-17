#include "D3DShader.h"

D3DVertexShader::D3DVertexShader(ID3DBlob* blob, ID3D11VertexShader* handle, ID3D11InputLayout* layout) :
    blob_(blob), handle_(handle), layout_(layout)
{
}

D3DVertexShader::~D3DVertexShader()
{
    blob_->Release();
    handle_->Release();
    layout_->Release();
}

void D3DVertexShader::Use(ID3D11DeviceContext* context)
{
    context->IASetInputLayout(layout_);
    context->VSSetShader(handle_, nullptr, 0u);
}


D3DPixelShader::D3DPixelShader(ID3DBlob* blob, ID3D11PixelShader* handle) :
    blob_(blob), handle_(handle)
{
}

D3DPixelShader::~D3DPixelShader()
{
    blob_->Release();
    handle_->Release();
}

void D3DPixelShader::Use(ID3D11DeviceContext* context)
{
    context->PSSetShader(handle_, nullptr, 0u);
}


D3DComputeShader::D3DComputeShader(ID3DBlob* blob, ID3D11ComputeShader* handle) :
    blob_(blob), handle_(handle)
{
}

D3DComputeShader::~D3DComputeShader()
{
    blob_->Release();
    handle_->Release();
    for (auto uav : uavs_)
        uav->Release();
    for (auto buffer : buffers_)
        buffer->Release();
}

void D3DComputeShader::AddUAV(ID3D11UnorderedAccessView* uav)
{
    uavs_.push_back(uav);
}

void D3DComputeShader::Invoke(ID3D11DeviceContext* context)
{
    context->CSSetShader(handle_, nullptr, 0u);
    if (!uavs_.empty())
    {
        context->CSSetUnorderedAccessViews(0u, uavs_.size(), uavs_.data(), nullptr);
    }

    // D3D11_BUFFER_DESC bufferDesc = {};
    // bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    // bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    // bufferDesc.CPUAccessFlags = 0u;
    // bufferDesc.MiscFlags = 0u;
    // bufferDesc.StructureByteStride = sizeof();
    // bufferDesc.ByteWidth =  * bufferDesc.StructureByteStride;

    // D3D11_SUBRESOURCE_DATA resourceData = {};
    // resourceData.pSysMem = data;

    // d3dAssert(mDevice->CreateBuffer(&bufferDesc, &resourceData, outBuffer->GetAddressOf()));
    // d3dAssert(device->CreateBuffer(&bufferDesc, initData != NULL ? &subresourceData : NULL, &Buffer));
        // context_->CSSetShaderResources()
    context->Dispatch(32, 32, 1);
    ID3D11UnorderedAccessView* null_uav = nullptr;
    context->CSSetUnorderedAccessViews(0u, 1u, &null_uav, nullptr);
}
