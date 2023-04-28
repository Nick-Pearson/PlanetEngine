#include "D3DVertexShader.h"

#include "d3dx12.h"

D3DVertexShader::D3DVertexShader(ID3DBlob* blob) :
    blob_(blob)
{
    ied_[0] = {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0};
    ied_[1] = {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0};
    ied_[2] = {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0};
}

D3DVertexShader::~D3DVertexShader()
{
    blob_->Release();
}


D3D12_INPUT_LAYOUT_DESC D3DVertexShader::GetInputLayout() const
{
    D3D12_INPUT_LAYOUT_DESC desc;
    desc.NumElements = _countof(ied_);
    desc.pInputElementDescs = ied_;
    return desc;
}