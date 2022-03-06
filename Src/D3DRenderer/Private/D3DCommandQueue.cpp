#include "D3DCommandQueue.h"

D3DCommandQueue::D3DCommandQueue(ID3D12Device2* device, D3D12_COMMAND_LIST_TYPE type, D3D12_COMMAND_QUEUE_PRIORITY priority) :
    next_signal_(16), last_completed_(0)
{
    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Priority = priority;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.Type = type;
    desc.NodeMask = 0;

    d3dAssert(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&command_queue_)));

    d3dAssert(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_)));
    event_ = ::CreateEvent(nullptr, false, false, nullptr);
}

D3DCommandQueue::~D3DCommandQueue()
{
    fence_->Release();
    fence_ == nullptr;
    command_queue_->Release();
    command_queue_ == nullptr;
    ::CloseHandle(event_);
}

void D3DCommandQueue::ExecuteCommandList(ID3D12GraphicsCommandList* command_list)
{
    d3dAssert(command_list->Close());
    ID3D12CommandList* const command_lists[] = {command_list};
    command_queue_->ExecuteCommandLists(_countof(command_lists), command_lists);
}

uint64_t D3DCommandQueue::Signal()
{
    uint64_t signal = ++next_signal_;
    d3dAssert(command_queue_->Signal(fence_, signal));
    return signal;
}

uint64_t D3DCommandQueue::UpdateAndGetLastCompletedSignal()
{
    last_completed_ = fence_->GetCompletedValue();
    return last_completed_;
}

void D3DCommandQueue::WaitForSignal(uint64_t signal)
{
    if (last_completed_ < signal)
    {
        last_completed_ = fence_->GetCompletedValue();
        if (last_completed_ < signal)
        {
            d3dAssert(fence_->SetEventOnCompletion(signal, event_));

            WaitForSingleObjectEx(event_, INFINITE, false);
            last_completed_ = signal;
        }
    }
}