#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

#include "Platform/PlanetWindows.h"
#include "Container/LinkedList.h"
#include "Render/ResourceManager.h"

class Mesh;
class Material;
class D3DShader;
class D3DTexture;

struct GPUMeshHandle
{
    std::shared_ptr<Mesh> mesh;
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> triangleBuffer;
    unsigned int numTriangles;

    bool operator==(const GPUMeshHandle& other) const
    {
        return other.mesh == mesh;
    }
};

struct GPUMaterialHandle
{
    std::shared_ptr<Material> material;
    std::shared_ptr<D3DShader> shader;
    std::vector<std::shared_ptr<D3DTexture>> textures;
    bool alpha;
};

class GPUResourceManager : public ResourceManager
{
 public:
    explicit GPUResourceManager(Microsoft::WRL::ComPtr <ID3D11Device> device);
    ~GPUResourceManager();

    GPUMeshHandle* LoadMesh(std::shared_ptr<Mesh> mesh) override;
    void UnloadMesh(std::shared_ptr<Mesh> mesh) override;

    std::shared_ptr<GPUMaterialHandle> LoadMaterial(std::shared_ptr<Material> material) override;
    void ReloadAllShaders() override;

 private:
    std::shared_ptr<D3DShader> LoadShader(const std::string& ShaderFile, bool force);

    void CreateBuffer(const void* data, size_t length, size_t stride, unsigned int flags, Microsoft::WRL::ComPtr<ID3D11Buffer>* outBuffer);

    LinkedList <GPUMeshHandle> mLoadedMeshes;
    std::unordered_map<std::string, std::shared_ptr<GPUMaterialHandle>> mLoadedMaterials;

    std::unordered_map<std::string, std::shared_ptr<D3DShader>> loadedShaders;

    Microsoft::WRL::ComPtr <ID3D11Device> mDevice;
};