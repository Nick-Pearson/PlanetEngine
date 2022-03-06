#include "D3DVertexShader.h"

D3DVertexShader::D3DVertexShader(ID3DBlob* blob) :
    blob_(blob)
{
}

D3DVertexShader::~D3DVertexShader()
{
    blob_->Release();
}