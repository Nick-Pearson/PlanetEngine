#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <wrl/client.h>

#include "Platform/PlanetWindows.h"
#include "Render/RenderSystem.h"
#include "Container/RingBuffer.h"
#include "D3DAssert.h"

#define NUM_BUFFERS 3

class D3DRenderSystem : public RenderSystem
{
 public:
    explicit D3DRenderSystem(HWND window);
    ~D3DRenderSystem();

    void Load(class PlanetEngine* engine) final;
    void UnLoad(class PlanetEngine* engine) final;

    Renderer* GetRenderer() const final;

    void ApplyQueue(const struct RenderQueueItems& items) final;

    void RenderFrame(const CameraComponent& camera) final;
    void RenderToTexture(Texture2D* texture, const CameraComponent& camera) final;

    void InvokeCompute(const ComputeShader* shader) final;

    void UpdateWindowSize();

 private:
    void Present();

    void RenderDebugUI();

    void InitDevice(HWND window);

    ID3D12Device2* CreateDevice();
    ID3D12CommandQueue* CreateCommandQueue(D3D12_COMMAND_LIST_TYPE type, D3D12_COMMAND_QUEUE_PRIORITY priority);
    IDXGISwapChain4* CreateSwapChain(HWND window);
    ID3D12DescriptorHeap* CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, int num_descriptors);

#if defined(DX_DEBUG)
    void FlushDebugMessages();

    ID3D12Debug* debug_ = nullptr;
    ID3D12InfoQueue* info_queue_ = nullptr;
#endif

    IDXGIAdapter4* adapter_ = nullptr;
    ID3D12Device2* device_ = nullptr;
    IDXGISwapChain4* swap_chain_ = nullptr;

    class D3DCommandQueue* draw_command_queue_ = nullptr;
    ID3D12GraphicsCommandList* draw_command_list_ = nullptr;
    ID3D12CommandAllocator* draw_command_allocator_ = nullptr;

    class D3DCommandQueue* compute_command_queue_ = nullptr;
    ID3D12GraphicsCommandList* compute_command_list_ = nullptr;
    ID3D12CommandAllocator* compute_command_allocator_ = nullptr;

    ID3D12DescriptorHeap* rtv_descriptor_heap_ = nullptr;
    ID3D12DescriptorHeap* dsv_descriptor_heap_ = nullptr;
    class SRVHeap* srv_heap_ = nullptr;
    class SRVHeap* compute_srv_heap_ = nullptr;

    class D3DRenderer* renderer_;
    class ImGUIRenderer* ui_renderer_;
    class GPUResourceManager* resource_manager_;
    class D3DWindowEvents* window_events_;
    
    class D3DComputeDispatch* compute_dispatch_;

    class WindowRenderTarget* window_render_target_;

    class BaseRootSignature* root_signature_;

    RingBuffer<uint64_t> frame_times_ms_{50};
    std::chrono::steady_clock::time_point last_present_time_;
};
