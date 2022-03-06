#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <wrl/client.h>

#include "Platform/PlanetWindows.h"
#include "Render/RenderSystem.h"
#include "Container/RingBuffer.h"

#define DX_DEBUG 1

#define NUM_BUFFERS 3

#if defined(DX_DEBUG)
    #define SET_NAME(obj, name) obj->SetName(L##name);
    #define SET_NAME_F(obj, name, ...) \
        { \
            wchar_t ___buff[256]; \
            swprintf(___buff, 256, L##name, ##__VA_ARGS__); \
            obj->SetName(___buff); \
        }
#else
    #define SET_NAME(obj, name)
    #define SET_NAME_F(obj, name, ...)
#endif

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

    void InvokeCompute(const ComputeShader& shader) final;

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

    ID3D12DescriptorHeap* rtv_descriptor_heap_ = nullptr;
    ID3D12DescriptorHeap* dsv_descriptor_heap_ = nullptr;

    class D3DRenderer* renderer_;
    class ImGUIRenderer* ui_renderer_;
    class GPUResourceManager* resource_manager_;
    class D3DWindowEvents* window_events_;

    class WindowRenderTarget* window_render_target_;

    RingBuffer<uint64_t> frame_times_ms_{50};
};
