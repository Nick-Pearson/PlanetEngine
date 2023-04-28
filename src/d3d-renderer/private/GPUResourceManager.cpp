#include "GPUResourceManager.h"

#include <string>

#include "d3dx12.h"

#include "D3DRenderer.h"
#include "Texture/D3DTexture.h"
#include "Mesh/Mesh.h"
#include "Mesh/D3DMesh.h"
#include "Material/Material.h"
#include "Material/D3DMaterial.h"
#include "Texture/Texture2D.h"
#include "Compute/ComputeShader.h"
#include "D3DCommandQueue.h"
#include "D3DRenderSystem.h"
#include "PlanetEngine.h"

#include "imgui.h"

void ResourceLoadBatch::OnLoadingComplete()
{
    for (auto mesh : pending_meshes_)
    {
        mesh->OnLoadingComplete();
    }
    pending_meshes_.clear();
    for (auto texture : pending_textures_)
    {
        texture->OnLoadingComplete();
    }
    pending_textures_.clear();
    for (auto material : pending_materials_)
    {
        material->OnLoadingComplete();
    }
    pending_materials_.clear();

    signal_ = EMPTY_BATCH;
}

GPUResourceManager::GPUResourceManager(ID3D12Device2* device, SRVHeap* srv_heap) :
    device_(device), srv_heap_(srv_heap)
{
    device_->AddRef();

    for (int i = 0; i < MAX_CONCURRENT_BATCHES; ++i)
    {
        ID3D12CommandAllocator* alloc = nullptr;
        d3dAssert(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&alloc)));
        SET_NAME_F(alloc, "Copy Allocator %d", i)

        batches_[i].command_allocator_ = alloc;
    }

    command_queue_ = new D3DCommandQueue{device_, D3D12_COMMAND_LIST_TYPE_COPY, D3D12_COMMAND_QUEUE_PRIORITY_NORMAL};
    SET_NAME(command_queue_, "Copy Command Queue")
    d3dAssert(device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COPY, batches_[0].command_allocator_, nullptr, IID_PPV_ARGS(&command_list_)));
    SET_NAME(command_list_, "Copy Command List")
    d3dAssert(command_list_->Close());

    texture_loader_ = new D3DTextureLoader{command_list_, device_};

    const VertexShader default_vert_shader{"VertexShader.hlsl"};
    default_d3d_vertex_shader_ = D3DShaderLoader::LoadVertex(&default_vert_shader);
}

GPUResourceManager::~GPUResourceManager()
{
    device_->Release();
    command_list_->Release();

    for (auto i = 0; i < MAX_CONCURRENT_BATCHES; ++i)
    {
        batches_[i].command_allocator_->Release();
    }
    delete command_queue_;

    delete texture_loader_;
}

D3DMesh* GPUResourceManager::LoadMesh(const Mesh* mesh)
{
    ResourceLoadBatch* batch = PrepareBatch();

    ID3D12Resource* vert_resource;
    ID3D12Resource* vert_intermediate_resource;
    CreateBuffer((const void*)mesh->GetVertexData(),
        mesh->GetVertexCount(),
        sizeof(Vertex),
        D3D12_RESOURCE_FLAG_NONE,
        &vert_resource,
        &vert_intermediate_resource);

    ID3D12Resource* tri_resource;
    ID3D12Resource* tri_intermediate_resource;
    CreateBuffer((const void*)mesh->GetTriangleData(),
        mesh->GetTriangleCount(),
        sizeof(uint16_t),
        D3D12_RESOURCE_FLAG_NONE,
        &tri_resource,
        &tri_intermediate_resource);

    D3DMesh* d3d_mesh = new D3DMesh{
        mesh,
        vert_resource, vert_intermediate_resource,
        tri_resource, tri_intermediate_resource
    };

    vert_resource->Release();
    vert_intermediate_resource->Release();
    tri_resource->Release();
    tri_intermediate_resource->Release();

    batch->pending_meshes_.push_back(d3d_mesh);

    return d3d_mesh;
}

D3DMaterial* GPUResourceManager::LoadMaterial(const Material* material)
{
    ResourceLoadBatch* batch = PrepareBatch();

    std::vector<const D3DTexture*> textures;
    int num_textures = material->GetNumTextures();
    for (int i = 0; i < num_textures; ++i)
    {
        const Texture* texture = material->GetTextureAt(i);
        auto loaded_texture = texture_loader_->Load(texture);
        if (loaded_texture)
        {
            textures.push_back(loaded_texture);
            batch->pending_textures_.push_back(loaded_texture);
        }
        else
        {
            P_FATAL("failed to load texture {}", (void*) texture)
        }
    }

    D3DDescriptorTable* descriptor_table = nullptr;
    if (!textures.empty())
        descriptor_table = srv_heap_->CreateDescriptorTable(textures.size(), textures.data());

    auto d3d_material = new D3DMaterial{
        descriptor_table,
        textures
    };
    batch->pending_materials_.push_back(d3d_material);
    return d3d_material;
}

D3DPipelineState* GPUResourceManager::CompilePipelineState(
    const VertexShader* vertex_shader,
    const PixelShader* pixel_shader,
    const D3DRootSignature* root_signature)
{
    auto d3d_pixel_shader = D3DShaderLoader::LoadPixel(pixel_shader);
    auto d3d_vertex_shader = vertex_shader == nullptr ? default_d3d_vertex_shader_ : D3DShaderLoader::LoadVertex(vertex_shader);

    auto pipeline_state = new D3DPipelineState{device_,
            root_signature->GetRootSignature(),
            d3d_vertex_shader,
            d3d_pixel_shader};
    pipeline_state->Compile();
    return pipeline_state;
}

void GPUResourceManager::ReloadAllShaders()
{
    P_LOG("Reloading all shaders");
}

ResourceLoadBatch* GPUResourceManager::PrepareBatch()
{
    ResourceLoadBatch* batch = &batches_[next_load_batch_];

    if (batch->IsInUse())
    {
        // the next batch is still executing on the GPU, wait for it to finish
        P_WARN("All resource batches in use, blocking resource loading");
        const auto start = std::chrono::steady_clock::now();
        command_queue_->WaitForSignal(batch->signal_);
        ProcessCompletedBatches();

        P_ASSERT(batch->IsEmpty(), "Resource batch not empty after freeing");
        auto time = std::chrono::steady_clock::now() - start;
        P_WARN("Took %dms to obtain resource batch", time/std::chrono::milliseconds(1));
    }

    if (batch->IsEmpty())
    {
        batch->signal_ = BUILDING_BATCH;

        batch->command_allocator_->Reset();
        d3dAssert(command_list_->Reset(batch->command_allocator_, nullptr));
    }
    return batch;
}

void GPUResourceManager::ProcessCompletedBatches()
{
    auto last_signal = command_queue_->UpdateAndGetLastCompletedSignal();
    for (auto i = 0; i < MAX_CONCURRENT_BATCHES; ++i)
    {
        ResourceLoadBatch& batch = batches_[i];
        if (batch.signal_ <= last_signal && batch.IsInUse())
        {
            batch.OnLoadingComplete();
        }
    }
}

std::shared_ptr<D3DComputeShader> GPUResourceManager::LoadCompute(const ComputeShader& shader)
{
    std::shared_ptr<D3DComputeShader> program = D3DShaderLoader::LoadCompute(shader);
    if (!program)
    {
        return nullptr;
    }

    // for (int slot = 0; slot < shader.GetNumTextureOutputs(); ++slot)
    // {
    //     const Texture* tex = shader.GetTextureOutput(slot);
    //     ID3D11UnorderedAccessView* uav = texture_loader_->LoadForCompute(tex);
    //     program->AddUAV(uav);
    // }

    // for (int slot = 0; slot < shader.GetNumDataInputs(); ++slot)
    // {
    //     const DataBuffer* data = shader.GetDataInput(slot);

    //     ID3D11Buffer* buff = nullptr;
    //     CreateBuffer(data->data_,
    //         data->count_,
    //         data->stride_,
    //         D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,
    //         D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
    //         &buff);

    //     D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
    //     desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
    //     desc.BufferEx.FirstElement = 0;
    //     desc.Format = DXGI_FORMAT_UNKNOWN;
    //     desc.BufferEx.NumElements = data->count_;
    //     ID3D11ShaderResourceView* res;
    //     d3dAssert(mDevice->CreateShaderResourceView(buff, &desc, &res));
    //     buff->Release();

    //     program->AddResource(res);
    // }
    return program;
}

void GPUResourceManager::RenderDebugUI()
{
    if (ImGui::Button("Reload all shaders"))
    {
        ReloadAllShaders();
    }
}

void GPUResourceManager::ExecuteResourceLoads()
{
    ResourceLoadBatch& batch = batches_[next_load_batch_];

    if (!batch.IsEmpty())
    {
        command_queue_->ExecuteCommandList(command_list_);
        batch.signal_ = command_queue_->Signal();
        next_load_batch_ = (next_load_batch_ + 1) % MAX_CONCURRENT_BATCHES;
    }
}

void GPUResourceManager::CreateBuffer(const void* data,
    size_t length,
    size_t stride,
    D3D12_RESOURCE_FLAGS flags,
    ID3D12Resource** resource,
    ID3D12Resource** intermediate_resource)
{
    const auto buffer_size = length * stride;

    d3dAssert(device_->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(buffer_size, flags),
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(resource)));
    d3dAssert(device_->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(buffer_size),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(intermediate_resource)));

    D3D12_SUBRESOURCE_DATA subresource_data = {};
    subresource_data.pData = data;
    subresource_data.RowPitch = buffer_size;
    subresource_data.SlicePitch = stride;
    UpdateSubresources(command_list_, *resource, *intermediate_resource, 0, 0, 1, &subresource_data);

    // D3D11_BUFFER_DESC bufferDesc = {};
    // bufferDesc.BindFlags = flags;
    // bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    // bufferDesc.CPUAccessFlags = 0u;
    // bufferDesc.MiscFlags = miscflags;
    // bufferDesc.StructureByteStride = stride;
    // bufferDesc.ByteWidth = length * bufferDesc.StructureByteStride;

    // D3D11_SUBRESOURCE_DATA resourceData = {};
    // resourceData.pSysMem = data;

    // d3dAssert(mDevice->CreateBuffer(&bufferDesc, &resourceData, outBuffer));
}