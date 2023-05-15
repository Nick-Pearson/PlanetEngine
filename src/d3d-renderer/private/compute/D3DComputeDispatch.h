#pragma once

#include <d3d12.h>

class ComputeShader;
class D3DCommandQueue;
class SRVHeap;
class D3DTextureLoader;

class D3DComputeDispatch
{
 public:
    D3DComputeDispatch(ID3D12Device2* device,
      SRVHeap* srv_heap,
      D3DCommandQueue* compute_command_queue,
      ID3D12GraphicsCommandList* command_list,
      ID3D12CommandAllocator* compute_command_allocator);
    ~D3DComputeDispatch();

    void Dispatch(const ComputeShader* shader);
 private:
    ID3D12Device2* device_;
    SRVHeap* srv_heap_;

    D3DCommandQueue* command_queue_;
    ID3D12GraphicsCommandList* command_list_;
    ID3D12CommandAllocator* command_allocator_;

    D3DTextureLoader* texture_loader_;
};