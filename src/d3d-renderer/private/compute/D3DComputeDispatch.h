#pragma once

#include <d3d12.h>

class ComputeShader;
class D3DCommandQueue;

class D3DComputeDispatch
{
 public:
    D3DComputeDispatch(ID3D12Device2* device,
      D3DCommandQueue* compute_command_queue,
      ID3D12GraphicsCommandList* command_list,
      ID3D12CommandAllocator* compute_command_allocator);

    void Dispatch(const ComputeShader* shader);
 private:
    ID3D12Device2* device_;

    D3DCommandQueue* command_queue_ = nullptr;
    ID3D12GraphicsCommandList* command_list_;
    ID3D12CommandAllocator* command_allocator_ = nullptr;
};