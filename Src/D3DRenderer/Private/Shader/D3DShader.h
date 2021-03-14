#pragma once

#include <d3d11.h>
#include <string>

class D3DVertexShader
{
 public:
    D3DVertexShader(ID3DBlob* blob, ID3D11VertexShader* handle, ID3D11InputLayout* layout);
    ~D3DVertexShader();

    void Use(ID3D11DeviceContext* context);

 private:
    ID3DBlob* blob_;
    ID3D11VertexShader* handle_;
    ID3D11InputLayout* layout_;
};

class D3DPixelShader
{
 public:
    D3DPixelShader(ID3DBlob* blob, ID3D11PixelShader* handle);
    ~D3DPixelShader();

    void Use(ID3D11DeviceContext* context);

 private:
    ID3DBlob* blob_;
    ID3D11PixelShader* handle_;
};

class D3DComputeShader
{
 public:
    D3DComputeShader(ID3DBlob* blob, ID3D11ComputeShader* handle);
    ~D3DComputeShader();

    void Use(ID3D11DeviceContext* context);
    void Invoke(ID3D11DeviceContext* context);

 private:
    ID3DBlob* blob_;
    ID3D11ComputeShader* handle_;
};