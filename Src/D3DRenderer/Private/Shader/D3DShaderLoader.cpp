#include "D3DShaderLoader.h"

#include <D3Dcompiler.h>
#include <codecvt>
#include <locale>

#include "D3DAssert.h"

namespace
{
    ID3DBlob* compile_shader_blob(const char* filepath, const char* target, const std::unordered_map<std::string, std::string>& defines)
    {
        std::wstring fullpath = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(filepath);
        fullpath.insert(0, L"./Shader/");

        const unsigned int compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_OPTIMIZATION_LEVEL1;

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

        ID3DBlob* shader_blob = nullptr;
        ID3DBlob* error_blob = nullptr;
        D3DCompileFromFile(fullpath.c_str(), &macros[0], nullptr, "main", target, compile_flags, 0u, &shader_blob, &error_blob);

        delete[] macros;

        if (shader_blob == nullptr)
        {
            if (error_blob)
            {
                const char* message = (const char*)error_blob->GetBufferPointer();
                P_ERROR("Failed to compile shader file: {}\n{}", filepath, message);
                error_blob->Release();
            }
            else
            {
                P_ERROR("Unkown error while compiling shader file: {}", filepath);
            }
            return nullptr;
        }

        if (error_blob)
        {
            const char* message = (const char*)error_blob->GetBufferPointer();
            P_WARN("Warnings while compiling shader file: {}\n{}", filepath, message);
            error_blob->Release();
        }

        return shader_blob;
    }
}  // namespace

const D3DVertexShader* D3DShaderLoader::LoadVertex(const char* filepath)
{
    P_LOG("Loading vertex shader {}", filepath);
    std::unordered_map<std::string, std::string> defines;
    ID3DBlob* blob = compile_shader_blob(filepath, "vs_5_1", defines);
    if (!blob)
    {
        return nullptr;
    }

    return new D3DVertexShader{ blob };
}

const D3DPixelShader* D3DShaderLoader::LoadPixel(const PixelShader* shader)
{
    P_LOG("Loading pixel shader {}", shader->GetShaderPath());
    std::unordered_map<std::string, std::string> defines;
    ID3DBlob* blob = compile_shader_blob(shader->GetShaderPath().c_str(), "ps_5_1", defines);
    if (!blob)
    {
        return nullptr;
    }
    return new D3DPixelShader{ blob };
}

std::shared_ptr<D3DComputeShader> D3DShaderLoader::LoadCompute(const ComputeShader& shader)
{
    P_LOG("Loading compute shader {}", shader.GetShaderName());
    ID3DBlob* blob = compile_shader_blob(shader.GetShaderName().c_str(), "cs_5_1", shader.GetDefines());
    if (!blob)
    {
        return nullptr;
    }
    // ID3D11ComputeShader* handle;
    // d3dAssert(device_->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &handle));
    // return std::make_shared<D3DComputeShader>(blob, handle, shader.GetNumThreads());
    return nullptr;
}