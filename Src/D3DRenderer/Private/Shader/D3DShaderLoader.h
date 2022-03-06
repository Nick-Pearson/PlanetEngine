#pragma once

#include <d3d12.h>
#include <memory>
#include <unordered_map>
#include <string>

#include "D3DVertexShader.h"
#include "D3DPixelShader.h"
#include "D3DComputeShader.h"

class D3DShaderLoader
{
 public:
    explicit D3DShaderLoader(ID3D12Device2* device);
    virtual ~D3DShaderLoader();

    const D3DVertexShader* LoadVertex(const char* filepath);
    std::shared_ptr<D3DPixelShader> LoadPixel(const char* filepath);
    std::shared_ptr<D3DComputeShader> LoadCompute(const class ComputeShader& shader);

 private:
    ID3DBlob* CompileShaderBlob(const char* filepath, const char* target, const std::unordered_map<std::string, std::string>& defines);

    ID3D12Device2* device_;
};