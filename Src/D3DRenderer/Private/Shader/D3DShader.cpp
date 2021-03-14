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
}

void D3DComputeShader::Use(ID3D11DeviceContext* context)
{
    context->CSSetShader(handle_, nullptr, 0u);
}

void D3DComputeShader::Invoke(ID3D11DeviceContext* context)
{
    context->Dispatch(32, 32, 1);
}
