#include "D3DComputeDispatch.h"

#include "d3dx12.h"

#include "D3DAssert.h"
#include "D3DComputeRootSignature.h"
#include "compute/ComputeShader.h"
#include "../shader/D3DShaderLoader.h"
#include "../D3DCommandQueue.h"

D3DComputeDispatch::D3DComputeDispatch(ID3D12Device2* device,
      D3DCommandQueue* command_queue,
      ID3D12GraphicsCommandList* command_list,
      ID3D12CommandAllocator* command_allocator) :
    device_(device),
    command_queue_(command_queue),
    command_list_(command_list),
    command_allocator_(command_allocator)
{
}

void D3DComputeDispatch::Dispatch(const ComputeShader* shader)
{
    command_allocator_->Reset();
    d3dAssert(command_list_->Reset(command_allocator_, nullptr));

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

    command_list_->SetPipelineState(pipeline_state);
    root_signature->Bind(command_list_);

    // const auto num_threads = shader->GetNumThreads();
    // command_list_->Dispatch(num_threads.x_, num_threads.y_, num_threads.z_);

    command_queue_->ExecuteCommandList(command_list_);
    auto signal = command_queue_->Signal();
    command_queue_->WaitForSignal(signal);

    pipeline_state->Release();
    delete root_signature;
    delete loaded_shader;
}