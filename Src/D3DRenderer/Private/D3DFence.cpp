#include "D3DFence.h"

D3DFence::D3DFence(ID3D12Device2* device, ID3D12CommandQueue* command_queue) :
    command_queue_(command_queue), next_signal_(16), last_completed_(0)
{
    command_queue_->AddRef();
    d3dAssert(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_)));
    event_ = ::CreateEvent(nullptr, false, false, nullptr);
}

D3DFence::~D3DFence()
{
    fence_->Release();
    fence_ == nullptr;
    command_queue_->Release();
    command_queue_ == nullptr;
    ::CloseHandle(event_);
}

uint64_t D3DFence::Signal()
{
    uint64_t signal = ++next_signal_;
    d3dAssert(command_queue_->Signal(fence_, signal));
    return signal;
}

void D3DFence::WaitForSignal(uint64_t signal)
{
    if (last_completed_ < signal)
    {
        last_completed_ = fence_->GetCompletedValue();
        if (last_completed_ < signal)
        {
            d3dAssert(fence_->SetEventOnCompletion(signal, event_));

            WaitForSingleObjectEx(event_, INFINITE, false);
        }
    }
}