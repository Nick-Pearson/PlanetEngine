#include "D3DShaderLoader.h"

#include <D3Dcompiler.h>
#include <codecvt>
#include <locale>

#include "Compute/ComputeShader.h"

#include "D3DAssert.h"

D3DShaderLoader::D3DShaderLoader(wrl::ComPtr<struct ID3D11Device> device) :
    device_(device)
{
}

std::shared_ptr<D3DVertexShader> D3DShaderLoader::LoadVertex(const char* filepath)
{
    P_LOG("Loading vertex shader {}", filepath);
    std::unordered_map<std::string, std::string> defines;
    ID3DBlob* blob = CompileShaderBlob(filepath, "vs_5_0", defines);
    if (!blob)
    {
        return nullptr;
    }
    ID3D11VertexShader* handle;
    d3dAssert(device_->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &handle));

    ID3D11InputLayout* layout;
    const D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        {"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12u, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24u, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    d3dAssert(device_->CreateInputLayout(ied, (UINT)std::size(ied), blob->GetBufferPointer(), blob->GetBufferSize(), &layout));

    return std::make_shared<D3DVertexShader>(blob, handle, layout);
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
    ID3D11PixelShader* handle;
    d3dAssert(device_->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &handle));
    return std::make_shared<D3DPixelShader>(blob, handle);
}

std::shared_ptr<D3DComputeShader> D3DShaderLoader::LoadCompute(const ComputeShader& shader)
{
    P_LOG("Loading compute shader {}", shader.GetShaderName());
    ID3DBlob* blob = CompileShaderBlob(shader.GetShaderName().c_str(), "cs_5_0", shader.GetDefines());
    if (!blob)
    {
        return nullptr;
    }
    ID3D11ComputeShader* handle;
    d3dAssert(device_->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &handle));
    return std::make_shared<D3DComputeShader>(blob, handle, shader.GetNumThreads());
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
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
    d3dAssert(D3DCompileFromFile(fullpath.c_str(), &macros[0], nullptr, "main", target, compileFlags, 0u, &shaderBlob, &errorBlob));

    delete[] macros;

    if (errorBlob)
    {
        P_ERROR("Failed to compile shader file: {}", filepath);
        const char* message = (const char*)errorBlob->GetBufferPointer();
        P_ERROR("{}", message);
        return nullptr;
    }
    else if (shaderBlob == nullptr)
    {
        P_ERROR("Unkown error while compiling shader file: {}", filepath);
    }
    return shaderBlob;
}