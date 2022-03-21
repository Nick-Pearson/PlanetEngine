#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

#include "Platform/PlanetWindows.h"
#include "Container/LinkedList.h"
#include "Texture/D3DTextureLoader.h"
#include "Shader/D3DShaderLoader.h"
#include "Mesh/MeshResource.h"
#include "Material/MaterialResource.h"

class Mesh;
class Material;
class D3DPixelShader;
class D3DTexture;
class D3DCommandQueue;

#define EMPTY_BATCH 0
#define BUILDING_BATCH 1
#define MIN_SIGNAL 16

struct ResourceLoadBatch
{
    uint64_t signal_ = EMPTY_BATCH;
    std::vector<class D3DMesh*> pending_meshes_;
    ID3D12CommandAllocator* command_allocator_;

    inline const bool IsInUse() const { return signal_ >= MIN_SIGNAL; }
    inline const bool IsEmpty() const { return signal_ == EMPTY_BATCH; }
};

#define MAX_CONCURRENT_BATCHES 16

class GPUResourceManager
{
 public:
    explicit GPUResourceManager(ID3D12Device2* device);
    ~GPUResourceManager();

    MeshResource* LoadMesh(const Mesh* mesh);

    MaterialResource* LoadMaterial(const Material* material);

    std::shared_ptr<D3DComputeShader> LoadCompute(const class ComputeShader& shader);

    void RenderDebugUI();

    void ExecuteResourceLoads();
    void ProcessCompletedBatches();

 private:
    void ReloadAllShaders();

    ResourceLoadBatch* PrepareBatch();

    const D3DPixelShader* LoadShader(const std::string& ShaderFile, bool force);

    void CreateBuffer(const void* data,
            size_t length,
            size_t stride,
            D3D12_RESOURCE_FLAGS flags,
            ID3D12Resource** resource,
            ID3D12Resource** intermediate_resource);

    std::unordered_map<std::string, const D3DPixelShader*> loadedShaders;

    ResourceLoadBatch batches_[MAX_CONCURRENT_BATCHES];
    size_t next_load_batch_ = 0;

    ID3D12Device2* device_;
    D3DCommandQueue* command_queue_;
    ID3D12GraphicsCommandList* command_list_;

    D3DTextureLoader* texture_loader_;
};