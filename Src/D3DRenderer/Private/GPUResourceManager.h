#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <string>
#include <memory>

#include "Platform/PlanetWindows.h"
#include "Container/LinkedList.h"
#include "Texture/D3DTextureLoader.h"
#include "Shader/D3DShaderLoader.h"
#include "Mesh/D3DMesh.h"
#include "Material/D3DMaterial.h"
#include "Descriptor/SRVHeap.h"

class Mesh;
class Material;
class VertexShader;
class PixelShader;
class D3DPipelineState;
class D3DRootSignature;
class D3DTexture;
class D3DCommandQueue;

#define EMPTY_BATCH 0
#define BUILDING_BATCH 1
#define MIN_SIGNAL 16

struct ResourceLoadBatch
{
    uint64_t signal_ = EMPTY_BATCH;
    std::vector<D3DMesh*> pending_meshes_;
    std::vector<D3DTexture*> pending_textures_;
    std::vector<D3DMaterial*> pending_materials_;
    ID3D12CommandAllocator* command_allocator_;

    inline const bool IsInUse() const { return signal_ >= MIN_SIGNAL; }
    inline const bool IsEmpty() const { return signal_ == EMPTY_BATCH; }
    void OnLoadingComplete();
};

#define MAX_CONCURRENT_BATCHES 16

class GPUResourceManager
{
 public:
    explicit GPUResourceManager(ID3D12Device2* device, SRVHeap* srv_heap);
    ~GPUResourceManager();

    D3DMesh* LoadMesh(const Mesh* mesh);

    D3DMaterial* LoadMaterial(const Material* material);

    D3DPipelineState* CompilePipelineState(
        const VertexShader* vertex_shader,
        const PixelShader* pixel_shader,
        const D3DRootSignature* root_signature);

    std::shared_ptr<D3DComputeShader> LoadCompute(const class ComputeShader& shader);

    void RenderDebugUI();

    void ExecuteResourceLoads();
    void ProcessCompletedBatches();

 private:
    void ReloadAllShaders();

    ResourceLoadBatch* PrepareBatch();

    void CreateBuffer(const void* data,
            size_t length,
            size_t stride,
            D3D12_RESOURCE_FLAGS flags,
            ID3D12Resource** resource,
            ID3D12Resource** intermediate_resource);

    ResourceLoadBatch batches_[MAX_CONCURRENT_BATCHES];
    size_t next_load_batch_ = 0;

    ID3D12Device2* const device_;
    SRVHeap* const srv_heap_;

    D3DCommandQueue* command_queue_;
    ID3D12GraphicsCommandList* command_list_;

    D3DTextureLoader* texture_loader_;

    const D3DVertexShader* default_d3d_vertex_shader_;
};