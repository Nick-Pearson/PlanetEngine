#include "D3DPixelShader.h"

D3DPixelShader::D3DPixelShader(const char* path, ID3DBlob* blob, ID3D11PixelShader* handle) :
    path_(path), blob_(blob), handle_(handle)
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
