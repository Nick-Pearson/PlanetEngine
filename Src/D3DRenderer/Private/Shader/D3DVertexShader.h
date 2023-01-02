#pragma once

#include <d3d12.h>

class D3DVertexShader
{
 public:
    explicit D3DVertexShader(ID3DBlob* blob);
    ~D3DVertexShader();

    inline ID3DBlob* GetBlob() const { return blob_; }
    D3D12_INPUT_LAYOUT_DESC GetInputLayout() const;

 private:
    D3D12_INPUT_ELEMENT_DESC ied_[3];
    ID3DBlob* const blob_;
};
