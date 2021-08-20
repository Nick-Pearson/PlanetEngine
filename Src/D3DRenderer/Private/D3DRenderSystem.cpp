#include "D3DRenderSystem.h"

#include <chrono>

#include "D3DRenderer.h"
#include "Target/TextureRenderTarget.h"
#include "Target/WindowRenderTarget.h"
#include "PlanetEngine.h"
#include "ImGUI/ImGUIRenderer.h"
#include "Render/RenderQueue.h"
#include "Entity/Entity.h"
#include "Mesh/MeshComponent.h"
#include "RenderState.h"
#include "D3DRenderer.h"
#include "GPUResourceManager.h"
#include "D3DWindowEvents.h"
#include "Compute/ComputeShader.h"
#include "D3DAssert.h"
#include "imgui.h"

namespace
{
    static const char* D3D_SEVERITY[] = {
            "CORRUPTION",
            "ERROR",
            "WARNING",
            "INFO",
            "MESSAGE"
    };
    static const char* D3D_CATEGORY[] = {
            "UNKNOWN",
            "MISCELLANEOUS",
            "INITIALIZATION",
            "CLEANUP",
            "COMPILATION",
            "STATE_CREATION",
            "STATE_SETTING",
            "STATE_GETTING",
            "RESOURCE_MANIPULATION",
            "EXECUTION",
            "SHADER"
    };
}  // namespace

namespace chr = std::chrono;

D3DRenderSystem::D3DRenderSystem(HWND window)
{
    const auto hModDxgiDebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    // get address of DXGIGetDebugInterface in dll
    typedef HRESULT (WINAPI* DXGIGetDebugInterface)(REFIID, IDXGIInfoQueue **);
    const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface"));
    DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), mDxgiInfoQueue.GetAddressOf());

    InitD3D11Device(window);
    mRenderer = new D3DRenderer{ mDevice, mSwapChain, mContext };
    mUIRenderer = new ImGUIRenderer{ window, mDevice, mContext };
    mResourceManager = new GPUResourceManager{ mDevice, mContext };
    mWindowEvents = new D3DWindowEvents{ this };
    window_render_target_ = new WindowRenderTarget{ mDevice.Get(), mSwapChain.Get() };
    mRenderer->BindRenderTarget(*window_render_target_);
}

D3DRenderSystem::~D3DRenderSystem()
{
    delete mRenderer;
    delete mUIRenderer;
    delete mResourceManager;
    delete mWindowEvents;
    delete window_render_target_;
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

void D3DRenderSystem::InvokeCompute(const ComputeShader& shader)
{
    std::shared_ptr<D3DComputeShader> loaded_shader = mResourceManager->LoadCompute(shader);
    if (loaded_shader)
    {
        loaded_shader->Invoke(mContext.Get());
    }
}

void D3DRenderSystem::UpdateWindowSize()
{
    window_render_target_->UpdateWindowSize(mDevice.Get());
    mRenderer->BindRenderTarget(*window_render_target_);
}

void D3DRenderSystem::FlushDebugMessages()
{
    const auto len = mDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_D3D11); \
    for (auto i = 0; i < len; ++i)
    {
        SIZE_T msg_len;
        mDxgiInfoQueue->GetMessage(DXGI_DEBUG_D3D11, i, nullptr, &msg_len);
        char* rawmsg = new char[msg_len];
        auto msg = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(rawmsg);
        mDxgiInfoQueue->GetMessage(DXGI_DEBUG_D3D11, i, msg, &msg_len);
        if (msg->Severity < DXGI_INFO_QUEUE_MESSAGE_SEVERITY_INFO)
        {
            P_LOG("D3D11DEBUG [{}] [{}] {}",
                D3D_SEVERITY[msg->Severity],
                D3D_CATEGORY[msg->Category],
                msg->pDescription);
        }
        delete rawmsg;
    }
}

void D3DRenderSystem::RenderFrame(const CameraComponent& camera)
{
    chr::high_resolution_clock::time_point start = chr::high_resolution_clock::now();
    mRenderer->Render(camera);
    RenderDebugUI();
    mUIRenderer->Render();
    window_render_target_->SwapBuffers();
    mUIRenderer->NewFrame();
    auto time = chr::high_resolution_clock::now() - start;
    frame_times_ms_.Add(time/chr::milliseconds(1));
    FlushDebugMessages();
}

void D3DRenderSystem::RenderToTexture(Texture2D* texture, const CameraComponent& camera)
{
    TextureRenderTarget texture_render_target{mDevice.Get(), texture->GetWidth(), texture->GetHeight()};

    mRenderer->BindRenderTarget(texture_render_target);

    mRenderer->Render(camera);
    texture_render_target.CopyToTexture(mDevice.Get(), mContext.Get(), texture);

    mRenderer->BindRenderTarget(*window_render_target_);

    FlushDebugMessages();
}

float calc_fps(uint64_t frame_time_ms)
{
    return frame_time_ms > 0 ? 1000.0f / static_cast<float>(frame_time_ms) : 0;
}

float get_fps_time(void* b, int idx)
{
    RingBuffer<uint64_t>* buffer = static_cast<RingBuffer<uint64_t>*>(b);
    return calc_fps((*buffer)[idx]);
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

    if (!frame_times_ms_.IsEmpty())
    {
        auto last_frame_time = frame_times_ms_.Head();
        auto average_frame_time = 0;
        for (int i = 0; i < frame_times_ms_.Capacity(); ++i)
            average_frame_time += frame_times_ms_[i];
        average_frame_time /= frame_times_ms_.Capacity();

        ImGui::Text("%.1f FPS (%d ms)", calc_fps(last_frame_time), last_frame_time);
        ImGui::Text("%.1f Average FPS (over last %d frames)", calc_fps(average_frame_time), frame_times_ms_.Capacity());
        ImGui::PlotLines("FPS", &get_fps_time, &frame_times_ms_, frame_times_ms_.Capacity(), 0, nullptr, 0.0f, 120.0f, ImVec2(200, 60));
    }

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
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
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
