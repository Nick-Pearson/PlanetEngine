#pragma once

#include <d3d12.h>
#include <vector>

struct DescriptorHandle
{
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_;
    D3D12_GPU_DESCRIPTOR_HANDLE gpu_;
};

class D3DDescriptorTable
{
 public:
    explicit D3DDescriptorTable(const DescriptorHandle& handle, size_t size);
    ~D3DDescriptorTable();

    void Bind(ID3D12GraphicsCommandList* command_list, uint32_t slot);

 private:
    const size_t size_;
    const DescriptorHandle handle_;
};
