#pragma once

#include <d3d11.h>
#include <vector>

#include "Compute/NumThreads.h"

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
    D3DComputeShader(ID3DBlob* blob, ID3D11ComputeShader* handle, const NumThreads& num_threads);
    ~D3DComputeShader();

    void Invoke(ID3D11DeviceContext* context);

    void AddUAV(ID3D11UnorderedAccessView* uav);
    void AddResource(ID3D11ShaderResourceView* res);

 private:
    ID3DBlob* blob_;
    ID3D11ComputeShader* handle_;
    NumThreads num_threads_;
    std::vector<ID3D11UnorderedAccessView*> uavs_;
    std::vector<ID3D11ShaderResourceView*> resources_;
};