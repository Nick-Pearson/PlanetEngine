#include "D3DShaderLoader.h"

#include <D3Dcompiler.h>
#include <codecvt>
#include <locale>

#include "Compute/ComputeShader.h"

#include "D3DAssert.h"

D3DShaderLoader::D3DShaderLoader(ID3D12Device2* device) :
    device_(device)
{
    device_->AddRef();
}

D3DShaderLoader::~D3DShaderLoader()
{
    device_->Release();
    device_ = nullptr;
}

const D3DVertexShader* D3DShaderLoader::LoadVertex(const char* filepath)
{
    P_LOG("Loading vertex shader {}", filepath);
    std::unordered_map<std::string, std::string> defines;
    ID3DBlob* blob = CompileShaderBlob(filepath, "vs_5_0", defines);
    if (!blob)
    {
        return nullptr;
    }

    return new D3DVertexShader{ blob };
}

std::shared_ptr<D3DPixelShader> D3DShaderLoader::LoadPixel(const char* filepath)
{
    P_LOG("Loading pixel shader {}", filepath);
    std::unordered_map<std::string, std::string> defines;
    ID3DBlob* blob = CompileShaderBlob(filepath, "ps_5_0", defines);
    if (!blob)
    {
        return nullptr;
    }
    // ID3D11PixelShader* handle;
    // d3dAssert(device_->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &handle));
    // return std::make_shared<D3DPixelShader>(filepath, blob, handle);
    return nullptr;
}

std::shared_ptr<D3DComputeShader> D3DShaderLoader::LoadCompute(const ComputeShader& shader)
{
    P_LOG("Loading compute shader {}", shader.GetShaderName());
    ID3DBlob* blob = CompileShaderBlob(shader.GetShaderName().c_str(), "cs_5_0", shader.GetDefines());
    if (!blob)
    {
        return nullptr;
    }
    // ID3D11ComputeShader* handle;
    // d3dAssert(device_->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &handle));
    // return std::make_shared<D3DComputeShader>(blob, handle, shader.GetNumThreads());
    return nullptr;
}

ID3DBlob* D3DShaderLoader::CompileShaderBlob(const char* filepath, const char* target, const std::unordered_map<std::string, std::string>& defines)
{
    std::wstring fullpath = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(filepath);
    fullpath.insert(0, L"./Shader/");

    const unsigned int compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_OPTIMIZATION_LEVEL1;

    D3D_SHADER_MACRO* macros = new D3D_SHADER_MACRO[defines.size() + 1]{};
    int i = 0;
    for (auto const& x : defines)
    {
        macros[i].Name = x.first.c_str();
        macros[i].Definition = x.second.c_str();
        ++i;
    }
    macros[i].Name = nullptr;
    macros[i].Definition = nullptr;

    ID3DBlob* shaderBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;
    d3dAssert(D3DCompileFromFile(fullpath.c_str(), &macros[0], nullptr, "main", target, compileFlags, 0u, &shaderBlob, &errorBlob));

    delete[] macros;

    if (errorBlob)
    {
        P_ERROR("Failed to compile shader file: {}", filepath);
        const char* message = (const char*)errorBlob->GetBufferPointer();
        P_ERROR("{}", message);
        errorBlob->Release();
        return nullptr;
    }
    else if (shaderBlob == nullptr)
    {
        P_ERROR("Unkown error while compiling shader file: {}", filepath);
    }
    return shaderBlob;
}