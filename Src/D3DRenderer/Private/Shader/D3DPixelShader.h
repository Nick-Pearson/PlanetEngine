#pragma once

#include <d3d11.h>
#include <string>

class D3DPixelShader
{
 public:
    D3DPixelShader(const char* path, ID3DBlob* blob, ID3D11PixelShader* handle);
    ~D3DPixelShader();

    void Use(ID3D11DeviceContext* context);
    inline const std::string GetPath() const { return path_; }

 private:
    std::string path_;
    ID3DBlob* blob_;
    ID3D11PixelShader* handle_;
};
