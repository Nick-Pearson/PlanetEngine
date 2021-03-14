#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <memory>

#include "D3DShader.h"

namespace wrl = Microsoft::WRL;

class D3DShaderLoader
{
 public:
    explicit D3DShaderLoader(wrl::ComPtr<struct ID3D11Device> device);

    std::shared_ptr<D3DVertexShader> LoadVertex(const char* filepath);
    std::shared_ptr<D3DPixelShader> LoadPixel(const char* filepath);
    std::shared_ptr<D3DComputeShader> LoadCompute(const char* filepath);

 private:
    ID3DBlob* CompileShaderBlob(const char* filepath, const char* target);

    wrl::ComPtr<ID3D11Device> device_;
};