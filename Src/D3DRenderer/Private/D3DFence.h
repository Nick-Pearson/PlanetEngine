#pragma once

#include <d3d12.h>

#include "D3DAssert.h"

class D3DFence
{
 public:
    D3DFence(ID3D12Device2* device, ID3D12CommandQueue* command_queue);
    virtual ~D3DFence();

    uint64_t Signal();

    void WaitForSignal(uint64_t signal);

 private:
    uint64_t next_signal_;
    uint64_t last_completed_;
    ID3D12Fence* fence_;
    ID3D12CommandQueue* command_queue_;
    HANDLE event_;
};