#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "D3DRenderer.h"
#include "ImGUI/ImGUIRenderer.h"
#include "GPUResourceManager.h"
#include "D3DWindowEvents.h"

#include "Platform/PlanetWindows.h"
#include "Render/RenderSystem.h"

class D3DRenderSystem : public RenderSystem
{
 public:
    explicit D3DRenderSystem(HWND window);
    ~D3DRenderSystem();

    void Load(class PlanetEngine* engine) override;
    void UnLoad(class PlanetEngine* engine) override;

    inline Renderer* GetRenderer() override
    {
        return mRenderer;
    }

    inline ResourceManager* GetResourceManager() override
    {
        return mResourceManager;
    }

    void RenderFrame(const CameraComponent& camera);

 private:
    void RenderDebugUI();
    void InitD3D11Device(HWND window);

    Microsoft::WRL::ComPtr <ID3D11Device> mDevice;
    Microsoft::WRL::ComPtr <IDXGISwapChain> mSwapChain;
    Microsoft::WRL::ComPtr <ID3D11DeviceContext> mContext;

    D3DRenderer* mRenderer;
    ImGUIRenderer* mUIRenderer;
    GPUResourceManager* mResourceManager;
    D3DWindowEvents* mWindowEvents;

    uint64_t mLastFrameMs;
};
