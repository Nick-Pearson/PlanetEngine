#pragma once

#include <d3d12.h>
#include <stdint.h>

struct DataBuffer;

class D3DDataBuffer
{
 public:
    D3DDataBuffer(ID3D12Device2* device, const DataBuffer* buffer);
    ~D3DDataBuffer();

    inline ID3D12Resource* GetResource() const { return resource_; }

    void Copy(ID3D12GraphicsCommandList* command_list) const;

    const size_t count_;
    const size_t stride_;
 private:
    ID3D12Device2* const device_;

    void* const cpu_data_;
    uint64_t total_size_;
    ID3D12Resource* resource_;
};