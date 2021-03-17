#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

#include "Platform/PlanetWindows.h"
#include "Container/LinkedList.h"
#include "Texture/D3DTextureLoader.h"
#include "Shader/D3DShaderLoader.h"

namespace wrl = Microsoft::WRL;

class Mesh;
class Material;
class D3DPixelShader;
class D3DTexture;

struct GPUMeshHandle
{
    wrl::ComPtr<ID3D11Buffer> vertexBuffer;
    wrl::ComPtr<ID3D11Buffer> triangleBuffer;
    unsigned int numTriangles;
};

struct GPUMaterialHandle
{
    std::shared_ptr<D3DPixelShader> shader;
    std::vector<std::shared_ptr<D3DTexture>> textures;
    bool alpha;
};

class GPUResourceManager
{
 public:
    explicit GPUResourceManager(wrl::ComPtr<ID3D11Device> device, wrl::ComPtr<ID3D11DeviceContext> context);
    ~GPUResourceManager();

    GPUMeshHandle* LoadMesh(const Mesh* mesh);

    std::shared_ptr<GPUMaterialHandle> LoadMaterial(const Material* material);
    void ReloadAllShaders();

    std::shared_ptr<D3DComputeShader> LoadCompute(const class ComputeShader& shader);

 private:
    std::shared_ptr<D3DPixelShader> LoadShader(const std::string& ShaderFile, bool force);

    void CreateBuffer(const void* data, size_t length, size_t stride, unsigned int flags, wrl::ComPtr<ID3D11Buffer>* outBuffer);

    LinkedList <GPUMeshHandle> mLoadedMeshes;
    std::unordered_map<std::string, std::shared_ptr<GPUMaterialHandle>> mLoadedMaterials;

    std::unordered_map<std::string, std::shared_ptr<D3DPixelShader>> loadedShaders;

    wrl::ComPtr<ID3D11Device> mDevice;

    D3DShaderLoader* shader_loader_;
    D3DTextureLoader* texture_loader_;
};