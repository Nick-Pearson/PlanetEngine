#include "D3DComputeShader.h"

D3DComputeShader::D3DComputeShader(ID3DBlob* blob) :
    blob_(blob)
{
}

D3DComputeShader::~D3DComputeShader()
{
    blob_->Release();
}
