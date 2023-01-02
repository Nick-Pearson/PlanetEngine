#include "D3DPixelShader.h"

D3DPixelShader::D3DPixelShader(ID3DBlob* blob) :
    blob_(blob)
{
}

D3DPixelShader::~D3DPixelShader()
{
    blob_->Release();
}
