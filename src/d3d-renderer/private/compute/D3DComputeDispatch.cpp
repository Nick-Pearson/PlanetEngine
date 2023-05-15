#include "D3DComputeDispatch.h"

#include <vector>
#include "d3dx12.h"

#include "D3DAssert.h"
#include "D3DComputeRootSignature.h"
#include "D3DDataBuffer.h"
#include "compute/ComputeShader.h"
#include "../descriptor/SRVHeap.h"
#include "../descriptor/D3DDescriptorTable.h"
#include "../shader/D3DShaderLoader.h"
#include "../texture/D3DTextureLoader.h"
#include "../D3DCommandQueue.h"

D3DComputeDispatch::D3DComputeDispatch(ID3D12Device2* device,
      SRVHeap* srv_heap,
      D3DCommandQueue* command_queue,
      ID3D12GraphicsCommandList* command_list,
      ID3D12CommandAllocator* command_allocator) :
    device_(device),
    srv_heap_(srv_heap),
    command_queue_(command_queue),
    command_list_(command_list),
    command_allocator_(command_allocator)
{
    device_->AddRef();
    command_list_->AddRef();
    command_allocator_->AddRef();

    command_allocator_->Reset();
    d3dAssert(command_list_->Reset(command_allocator_, nullptr));

    texture_loader_ = new D3DTextureLoader{ command_list, device };
}

D3DComputeDispatch::~D3DComputeDispatch()
{
    device_->Release();
    srv_heap_ = nullptr;
    command_queue_ = nullptr;
    command_list_->Release();
    command_allocator_->Release();
}

void D3DComputeDispatch::Dispatch(const ComputeShader* shader)
{
    const D3DComputeShader* loaded_shader = D3DShaderLoader::LoadCompute(shader);

    D3DComputeRootSignature* root_signature = new D3DComputeRootSignature{ shader, device_ };

    struct PipelineStateStream
    {
        CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
        CD3DX12_PIPELINE_STATE_STREAM_CS CS;
    } state;
    state.pRootSignature = root_signature->GetRootSignature();
    state.CS = CD3DX12_SHADER_BYTECODE(loaded_shader->GetBlob());

    ID3D12PipelineState* pipeline_state = nullptr;
    D3D12_PIPELINE_STATE_STREAM_DESC desc = { sizeof(PipelineStateStream), &state };
    d3dAssert(device_->CreatePipelineState(&desc, IID_PPV_ARGS(&pipeline_state)));

    std::vector<D3DTexture*> textures;
    int num_textures = shader->GetNumTextureOutputs();
    for (int i = 0; i < num_textures; ++i)
    {
        textures.push_back(texture_loader_->Load(shader->GetTextureOutput(i)));
    }
    D3DDescriptorTable* texture_descriptor_table = srv_heap_->CreateUAVDescriptorTable(
        textures.size(), const_cast<const D3DTexture**>(textures.data()));

    int num_data_inputs = shader->GetNumDataInputs();
    std::vector<const D3DDataBuffer*> buffers;
    for (int i = 0; i < num_data_inputs; ++i)
    {
        D3DDataBuffer* d3d_buffer = new D3DDataBuffer{ device_, shader->GetDataInput(i) };
        buffers.push_back(d3d_buffer);
    }
    D3DDescriptorTable* buffer_descriptor_table = srv_heap_->CreateSRVDescriptorTable(buffers.size(), buffers.data());

    for (auto d3d_buffer : buffers)
    {
        d3d_buffer->Copy(command_list_);
    }

    srv_heap_->Bind(command_list_);
    command_list_->SetPipelineState(pipeline_state);
    root_signature->Bind(command_list_);
    texture_descriptor_table->BindCompute(command_list_, 0U);

    // const auto num_threads = shader->GetNumThreads();
    // command_list_->Dispatch(num_threads.x_, num_threads.y_, num_threads.z_);

    command_queue_->ExecuteCommandList(command_list_);

    auto signal = command_queue_->Signal();
    command_queue_->WaitForSignal(signal);
    command_allocator_->Reset();
    d3dAssert(command_list_->Reset(command_allocator_, nullptr));

    for (auto texture : textures)
    {
        texture->OnLoadingComplete();
    }

    pipeline_state->Release();
    delete root_signature;
    delete loaded_shader;
    delete texture_descriptor_table;
}