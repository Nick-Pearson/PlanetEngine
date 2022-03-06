#pragma once

#include <d3d12.h>

class D3DVertexShader
{
 public:
    explicit D3DVertexShader(ID3DBlob* blob);
    ~D3DVertexShader();

    inline ID3DBlob* GetBlob() const { return blob_; }

 private:
    ID3DBlob* blob_;
};
