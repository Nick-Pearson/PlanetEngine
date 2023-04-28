#pragma once

#include <d3d12.h>

#include "../Texture/D3DTexture.h"
#include "D3DDescriptorTable.h"

class SRVHeap
{
 public:
    explicit SRVHeap(ID3D12Device2* device);
    ~SRVHeap();

    inline ID3D12DescriptorHeap* GetDescriptorHeap() const { return descriptor_heap_; }

    D3DDescriptorTable* CreateDescriptorTable(size_t num_textures, const D3DTexture** textures);
    DescriptorHandle CreateDescriptorHandle();

    void Bind(ID3D12GraphicsCommandList* command_list);
 private:
    const size_t capacity_ = 4096;
    size_t size_ = 0;

    ID3D12Device2* const device_;
    ID3D12DescriptorHeap* descriptor_heap_;
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle_;
    D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle_;
};
