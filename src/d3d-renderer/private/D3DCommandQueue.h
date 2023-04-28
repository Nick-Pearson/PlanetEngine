#pragma once

#include <d3d12.h>

#include "D3DAssert.h"

class D3DCommandQueue
{
 public:
    D3DCommandQueue(ID3D12Device2* device, D3D12_COMMAND_LIST_TYPE type, D3D12_COMMAND_QUEUE_PRIORITY priority);
    virtual ~D3DCommandQueue();

    inline ID3D12CommandQueue* GetQueue() const { return command_queue_; }
    void ExecuteCommandList(ID3D12GraphicsCommandList* command_list);

    void SetName(wchar_t* name);

    uint64_t Signal();

    // fetching the current fence state from the gpu is potentially expensive
    uint64_t UpdateAndGetLastCompletedSignal();
    inline uint64_t GetLastCompletedSignal() const { return last_completed_; }

    void WaitForSignal(uint64_t signal);

 private:
    uint64_t next_signal_;
    uint64_t last_completed_;
    ID3D12Fence* fence_;
    ID3D12CommandQueue* command_queue_;
    HANDLE event_;
};