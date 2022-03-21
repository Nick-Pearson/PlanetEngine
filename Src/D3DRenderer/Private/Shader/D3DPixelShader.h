#pragma once

#include <d3d11.h>
#include <string>

class D3DPixelShader
{
 public:
    explicit D3DPixelShader(ID3DBlob* blob);
    ~D3DPixelShader();

    inline ID3DBlob* GetBlob() const { return blob_; }

 private:
    std::string path_;
    ID3DBlob* blob_;
};
