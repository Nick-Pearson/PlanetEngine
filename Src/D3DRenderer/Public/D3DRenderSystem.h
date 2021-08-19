#pragma once

#include <d3d11.h>
#include <dxgidebug.h>
#include <wrl/client.h>

#include "Platform/PlanetWindows.h"
#include "Render/RenderSystem.h"
#include "Container/RingBuffer.h"

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

 private:
    void RenderDebugUI();
    void InitD3D11Device(HWND window);
    void FlushDebugMessages();

    Microsoft::WRL::ComPtr<ID3D11Device> mDevice;
    Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> mContext;
    Microsoft::WRL::ComPtr<IDXGIInfoQueue> mDxgiInfoQueue;

    class D3DRenderer* mRenderer;
    class ImGUIRenderer* mUIRenderer;
    class GPUResourceManager* mResourceManager;
    class D3DWindowEvents* mWindowEvents;

    RingBuffer<uint64_t> frame_times_ms_{50};
};
