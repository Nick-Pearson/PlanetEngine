#include "D3DRenderSystem.h"

#include <chrono>

#include "D3DRenderer.h"
#include "PlanetEngine.h"
#include "ImGUI/ImGUIRenderer.h"
#include "Render/RenderQueue.h"
#include "Entity/Entity.h"
#include "Mesh/MeshComponent.h"
#include "RenderState.h"
#include "D3DRenderer.h"
#include "GPUResourceManager.h"
#include "D3DWindowEvents.h"
#include "imgui.h"

namespace chr = std::chrono;

D3DRenderSystem::D3DRenderSystem(HWND window)
{
    InitD3D11Device(window);
    mRenderer = new D3DRenderer{ window, mDevice, mSwapChain, mContext };
    mUIRenderer = new ImGUIRenderer{ window, mDevice, mContext };
    mResourceManager = new GPUResourceManager{ mDevice };
    mWindowEvents = new D3DWindowEvents{ mRenderer };
}

D3DRenderSystem::~D3DRenderSystem()
{
    delete mRenderer;
    delete mUIRenderer;
    delete mResourceManager;
    delete mWindowEvents;
}

void D3DRenderSystem::Load(class PlanetEngine* engine)
{
    engine->RegisterMessageHandler(mWindowEvents);
}

void D3DRenderSystem::UnLoad(class PlanetEngine* engine)
{
    engine->UnregisterMessageHandler(mWindowEvents);
}

Renderer* D3DRenderSystem::GetRenderer() const
{
    return mRenderer;
}

void D3DRenderSystem::ApplyQueue(const RenderQueueItems& items)
{
    for (auto mesh : items.new_meshes)
    {
        RenderState renderState;
        renderState.debugName = mesh->GetParent()->GetName();
        renderState.UseDepthBuffer = mesh->render_config_.use_depth_buffer;
        renderState.UseWorldMatrix = mesh->render_config_.use_world_matrix;
        renderState.mesh = mResourceManager->LoadMesh(mesh->GetMesh());
        renderState.material = mResourceManager->LoadMaterial(mesh->GetMaterial());
        renderState.model = mesh->GetWorldTransform();

        mRenderer->AddRenderState(renderState);
    }
}

void D3DRenderSystem::RenderFrame(const CameraComponent& camera)
{
    chr::high_resolution_clock::time_point start = chr::high_resolution_clock::now();
    mRenderer->Render(camera);
    RenderDebugUI();
    mUIRenderer->Render();
    mRenderer->SwapBuffers();
    mUIRenderer->NewFrame();
    auto time = chr::high_resolution_clock::now() - start;
    mLastFrameMs = time/chr::milliseconds(1);
}

void D3DRenderSystem::RenderDebugUI()
{
    ImGui::Begin("Rendering");
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    BOOL fullscreen;
    d3dAssert(mSwapChain->GetDesc(&swapChainDesc));
    d3dAssert(mSwapChain->GetFullscreenState(&fullscreen, nullptr));

    ImGui::Text("%d x %d %s",
        swapChainDesc.BufferDesc.Width,
        swapChainDesc.BufferDesc.Height,
        fullscreen ? "(fullscreen)" : "");
    auto fps = mLastFrameMs > 0 ? 1000 / mLastFrameMs : 0;
    ImGui::Text("%d FPS (%d ms)", fps, mLastFrameMs);
    if (ImGui::Button("Reload all shaders"))
    {
        mResourceManager->ReloadAllShaders();
    }
    ImGui::End();
}

void D3DRenderSystem::InitD3D11Device(HWND window)
{
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 0;
    sd.BufferDesc.RefreshRate.Denominator = 0;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 1;
    sd.OutputWindow = window;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = 0;

    // create device and front/back buffers, and swap chain and rendering context
    unsigned int createFlags = D3D11_CREATE_DEVICE_DEBUG;

    D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        createFlags,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &sd,
        mSwapChain.GetAddressOf(),
        mDevice.GetAddressOf(),
        nullptr,
        mContext.GetAddressOf());
}
