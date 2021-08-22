#include "GPUResourceManager.h"

#include <string>

#include "D3DRenderer.h"
#include "Texture/D3DTexture.h"
#include "Mesh/Mesh.h"
#include "Material/Material.h"
#include "Texture/Texture2D.h"
#include "Compute/ComputeShader.h"

#include "imgui.h"

GPUResourceManager::GPUResourceManager(wrl::ComPtr<ID3D11Device> device, wrl::ComPtr<ID3D11DeviceContext> context) :
    mDevice(device)
{
    shader_loader_ = new D3DShaderLoader{device};
    texture_loader_ = new D3DTextureLoader{device, context, shader_loader_};
}

GPUResourceManager::~GPUResourceManager()
{
    delete texture_loader_;
    delete shader_loader_;
}

GPUMeshHandle* GPUResourceManager::LoadMesh(const Mesh* mesh)
{
    if (!mesh) return nullptr;

    GPUMeshHandle entry;
    entry.numTriangles = mesh->mTriangles.size();

    // load the buffers
    CreateBuffer((const void*)mesh->mVerticies.data(),
        mesh->mVerticies.size(),
        sizeof(Vertex),
        D3D11_BIND_VERTEX_BUFFER,
        0u,
        entry.vertexBuffer.GetAddressOf());
    CreateBuffer((const void*)mesh->mTriangles.data(),
        mesh->mTriangles.size(),
        sizeof(uint16_t),
        D3D11_BIND_INDEX_BUFFER,
        0u,
        entry.triangleBuffer.GetAddressOf());

    return mLoadedMeshes.Add(entry);
}

std::shared_ptr<GPUMaterialHandle> GPUResourceManager::LoadMaterial(const Material* material)
{
    auto existing = mLoadedMaterials.find(material->GetShaderPath());
    if (existing != mLoadedMaterials.end())
    {
        P_LOG("Reusing existing loaded material for {}", material->GetShaderPath());
        return existing->second;
    }

    auto loaded_shader = LoadShader(material->GetShaderPath(), false);

    std::shared_ptr<GPUMaterialHandle> entry = std::make_shared<GPUMaterialHandle>();
    entry->shader = loaded_shader;
    entry->alpha = material->IsAlphaBlendingEnabled();

    int numTextures = material->GetNumTextures();
    for (int i = 0; i < numTextures; ++i)
    {
        const Texture* texture = material->GetTextureAt(i);
        auto loaded_texture = texture_loader_->Load(texture);
        if (loaded_texture)
        {
            entry->textures.push_back(loaded_texture);
        }
        else
        {
            P_FATAL("failed to load texture {}", (void*) texture)
        }
    }

    mLoadedMaterials.emplace(material->GetShaderPath(), entry);
    return entry;
}

void GPUResourceManager::ReloadAllShaders()
{
    P_LOG("Reloading all shaders");
    for (auto i : mLoadedMaterials)
    {
        i.second->shader = LoadShader(i.first, true);
    }
}

std::shared_ptr<D3DComputeShader> GPUResourceManager::LoadCompute(const ComputeShader& shader)
{
    std::shared_ptr<D3DComputeShader> program = shader_loader_->LoadCompute(shader);
    if (!program)
    {
        return nullptr;
    }

    for (int slot = 0; slot < shader.GetNumTextureOutputs(); ++slot)
    {
        const Texture* tex = shader.GetTextureOutput(slot);
        ID3D11UnorderedAccessView* uav = texture_loader_->LoadForCompute(tex);
        program->AddUAV(uav);
    }

    for (int slot = 0; slot < shader.GetNumDataInputs(); ++slot)
    {
        const DataBuffer* data = shader.GetDataInput(slot);

        ID3D11Buffer* buff = nullptr;
        CreateBuffer(data->data_,
            data->count_,
            data->stride_,
            D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,
            D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
            &buff);

        D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
        desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
        desc.BufferEx.FirstElement = 0;
        desc.Format = DXGI_FORMAT_UNKNOWN;
        desc.BufferEx.NumElements = data->count_;
        ID3D11ShaderResourceView* res;
        d3dAssert(mDevice->CreateShaderResourceView(buff, &desc, &res));
        buff->Release();

        program->AddResource(res);
    }
    return program;
}

void GPUResourceManager::RenderDebugUI()
{
    if (ImGui::Button("Reload all shaders"))
    {
        ReloadAllShaders();
    }
}

std::shared_ptr<D3DPixelShader> GPUResourceManager::LoadShader(const std::string& shaderFile, bool force)
{
    if (!force)
    {
        auto it = loadedShaders.find(shaderFile);
        if (it != loadedShaders.end())
        {
            return it->second;
        }
    }

    auto loaded_shader = shader_loader_->LoadPixel(shaderFile.c_str());
    if (!loaded_shader)
    {
        P_ERROR("failed to load shader {}", shaderFile);
        loaded_shader = shader_loader_->LoadPixel("FallbackShader.hlsl");
    }

    loadedShaders.emplace(shaderFile, loaded_shader);
    return loaded_shader;
}

void GPUResourceManager::CreateBuffer(const void* data,
    size_t length,
    size_t stride,
    unsigned int flags,
    unsigned int miscflags,
    ID3D11Buffer** outBuffer)
{
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.BindFlags = flags;
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.CPUAccessFlags = 0u;
    bufferDesc.MiscFlags = miscflags;
    bufferDesc.StructureByteStride = stride;
    bufferDesc.ByteWidth = length * bufferDesc.StructureByteStride;

    D3D11_SUBRESOURCE_DATA resourceData = {};
    resourceData.pSysMem = data;

    d3dAssert(mDevice->CreateBuffer(&bufferDesc, &resourceData, outBuffer));
}