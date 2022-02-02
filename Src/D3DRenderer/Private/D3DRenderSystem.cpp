#include "D3DRenderSystem.h"

#include <stdlib.h>
#include <chrono>

#include "d3dx12.h"

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
#include "Material/Material.h"
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
#if defined(DX_DEBUG)
    {
        d3dAssert(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_)));
        debug_->EnableDebugLayer();
    }
#endif

    InitDevice(window);


    ui_renderer_ = new ImGUIRenderer{ window };
    // mResourceManager = new GPUResourceManager{ mDevice, mContext };
    window_events_ = new D3DWindowEvents{ this };

    // Material wireframe_material{"WireframeShader.hlsl"};
    // auto wireframe_shader = mResourceManager->LoadMaterial(&wireframe_material);
    renderer_ = new D3DRenderer{ draw_command_list_ };
    window_render_target_ = new WindowRenderTarget{device_, swap_chain_, rtv_descriptor_heap_, draw_command_queue_};
    renderer_->BindRenderTarget(window_render_target_);

#if defined(DX_DEBUG)
    FlushDebugMessages();
#endif
}

D3DRenderSystem::~D3DRenderSystem()
{
    delete renderer_;
    delete ui_renderer_;
    delete mResourceManager;
    delete window_events_;
    delete window_render_target_;

    draw_command_queue_->Release();
    draw_command_allocator_->Release();
    draw_command_list_->Release();
    compute_command_queue_->Release();
    loading_command_queue_->Release();

    rtv_descriptor_heap_->Release();

    device_->Release();
    adapter_->Release();
}

void D3DRenderSystem::Load(class PlanetEngine* engine)
{
    engine->RegisterMessageHandler(window_events_);
    ui_renderer_->NewFrame();
}

void D3DRenderSystem::UnLoad(class PlanetEngine* engine)
{
    engine->UnregisterMessageHandler(window_events_);
}

Renderer* D3DRenderSystem::GetRenderer() const
{
    return renderer_;
}

void D3DRenderSystem::ApplyQueue(const RenderQueueItems& items)
{
    for (auto mesh : items.new_meshes)
    {
        RenderState renderState;
        renderState.debugName = mesh->GetParent()->GetName();
        renderState.UseDepthBuffer = mesh->render_config_.use_depth_buffer;
        renderState.UseWorldMatrix = mesh->render_config_.use_world_matrix;
        // renderState.mesh = mResourceManager->LoadMesh(mesh->GetMesh());
        // renderState.material = mResourceManager->LoadMaterial(mesh->GetMaterial());
        renderState.model = mesh->GetWorldTransform();

        renderer_->AddRenderState(renderState);
    }
}

void D3DRenderSystem::InvokeCompute(const ComputeShader& shader)
{
    // std::shared_ptr<D3DComputeShader> loaded_shader = mResourceManager->LoadCompute(shader);
    // if (loaded_shader)
    // {
        // loaded_shader->Invoke(mContext.Get());
    // }
}

void D3DRenderSystem::UpdateWindowSize()
{
    // mRenderer->UnbindRenderTarget();
    // window_render_target_->UpdateWindowSize(mDevice.Get());
    // mRenderer->BindRenderTarget(*window_render_target_);
}

#if defined(DX_DEBUG)
void D3DRenderSystem::FlushDebugMessages()
{
    // const auto len = debug_->GetNumStoredMessages(DXGI_DEBUG_D3D12); \
    // for (auto i = 0; i < len; ++i)
    // {
    //     SIZE_T msg_len;
    //     dxgi_info_queue_->GetMessage(DXGI_DEBUG_D3D12, i, nullptr, &msg_len);
    //     char* rawmsg = new char[msg_len];
    //     auto msg = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(rawmsg);
    //     dxgi_info_queue_->GetMessage(DXGI_DEBUG_D3D12, i, msg, &msg_len);
    //     if (msg->Severity < DXGI_INFO_QUEUE_MESSAGE_SEVERITY_INFO)
    //     {
    //         P_LOG("D3D12DEBUG [{}] [{}] {}",
    //             D3D_SEVERITY[msg->Severity],
    //             D3D_CATEGORY[msg->Category],
    //             msg->pDescription);
    //     }
    //     delete rawmsg;
    // }
}
#endif

void D3DRenderSystem::RenderFrame(const CameraComponent& camera)
{
    chr::high_resolution_clock::time_point start = chr::high_resolution_clock::now();
    window_render_target_->PreRender();
    renderer_->Render(camera);
    // RenderDebugUI();
    // mUIRenderer->Render();

    Present();
    // mUIRenderer->NewFrame();
    auto time = chr::high_resolution_clock::now() - start;
    frame_times_ms_.Add(time/chr::milliseconds(1));
    FlushDebugMessages();
}

void D3DRenderSystem::Present()
{
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(window_render_target_->GetRenderTarget()->resource_,
        D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    draw_command_list_->ResourceBarrier(1, &barrier);

    d3dAssert(draw_command_list_->Close());

    ID3D12CommandList* const command_lists[] = {draw_command_list_};
    draw_command_queue_->ExecuteCommandLists(_countof(command_lists), command_lists);

    window_render_target_->Present();
}

void D3DRenderSystem::RenderToTexture(Texture2D* texture, const CameraComponent& camera)
{
    // TextureRenderTarget texture_render_target{mDevice.Get(), texture->GetWidth(), texture->GetHeight()};

    // mRenderer->BindRenderTarget(texture_render_target);

    // mRenderer->Render(camera);
    // texture_render_target.CopyToTexture(mDevice.Get(), mContext.Get(), texture);

    // mRenderer->BindRenderTarget(*window_render_target_);

    // FlushDebugMessages();
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
    // ImGui::Begin("Rendering");
    // DXGI_SWAP_CHAIN_DESC swapChainDesc;
    // BOOL fullscreen;
    // d3dAssert(mSwapChain->GetDesc(&swapChainDesc));
    // d3dAssert(mSwapChain->GetFullscreenState(&fullscreen, nullptr));

    // ImGui::Text("%d x %d %s",
    //     swapChainDesc.BufferDesc.Width,
    //     swapChainDesc.BufferDesc.Height,
    //     fullscreen ? "(fullscreen)" : "");

    // if (!frame_times_ms_.IsEmpty())
    // {
    //     auto last_frame_time = frame_times_ms_.Head();
    //     auto average_frame_time = 0;
    //     for (int i = 0; i < frame_times_ms_.Capacity(); ++i)
    //         average_frame_time += frame_times_ms_[i];
    //     average_frame_time /= frame_times_ms_.Capacity();

    //     ImGui::Text("%.1f FPS (%d ms)", calc_fps(last_frame_time), last_frame_time);
    //     ImGui::Text("%.1f Average FPS (over last %d frames)", calc_fps(average_frame_time), frame_times_ms_.Capacity());
    //     ImGui::PlotLines("FPS", &get_fps_time, &frame_times_ms_, frame_times_ms_.Capacity(), 0, nullptr, 0.0f, 120.0f, ImVec2(200, 60));
    // }

    // mResourceManager->RenderDebugUI();
    // mRenderer->RenderDebugUI();

    // ImGui::End();
}

void D3DRenderSystem::InitDevice(HWND window)
{
    device_ = CreateDevice();

    draw_command_queue_ = CreateCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
    d3dAssert(device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&draw_command_allocator_)));
    d3dAssert(device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, draw_command_allocator_, nullptr, IID_PPV_ARGS(&draw_command_list_)));
    d3dAssert(draw_command_list_->Close());

    compute_command_queue_ = CreateCommandQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE);
    loading_command_queue_ = CreateCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);

    swap_chain_ = CreateSwapChain(window);

    rtv_descriptor_heap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, NUM_BUFFERS);
}

ID3D12Device2* D3DRenderSystem::CreateDevice()
{
    IDXGIFactory6* factory6 = nullptr;
    UINT createFactoryFlags = 0;
#if defined(DX_DEBUG)
    createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
    d3dAssert(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&factory6)));

    HRESULT result = factory6->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter_));
    P_ASSERT(result != DXGI_ERROR_NOT_FOUND, "No compatible graphics devices found")

    DXGI_ADAPTER_DESC1 desc;
    adapter_->GetDesc1(&desc);

    char description_buffer[128];
    wcstombs(description_buffer, desc.Description, sizeof(description_buffer));
    P_LOG("Initialising graphics device [{}]", description_buffer);

    ID3D12Device2* device = nullptr;
    d3dAssert(D3D12CreateDevice(adapter_, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)));
    SET_NAME(device, "Device")

    #if defined(DX_DEBUG)
    {
        // d3dAssert(device->QueryInterface(&info_queue_));

        // info_queue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        // info_queue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        // info_queue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
    }
    #endif

    factory6->Release();
    return device;
}

ID3D12CommandQueue* D3DRenderSystem::CreateCommandQueue(D3D12_COMMAND_LIST_TYPE type)
{
    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.Type = type;
    desc.NodeMask = 0;

    ID3D12CommandQueue* command_queue = nullptr;
    d3dAssert(device_->CreateCommandQueue(&desc, IID_PPV_ARGS(&command_queue)));
    return command_queue;
}

IDXGISwapChain4* D3DRenderSystem::CreateSwapChain(HWND window)
{
    IDXGIFactory6* factory6 = nullptr;
    UINT createFactoryFlags = 0;
#if defined(DX_DEBUG)
    createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
    d3dAssert(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&factory6)));

    DXGI_SWAP_CHAIN_DESC1 desc = {};
    desc.Width = 1280;
    desc.Height = 720;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.Stereo = false;
    desc.SampleDesc = { 1, 0 };
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 3;
    desc.Scaling = DXGI_SCALING_STRETCH;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    desc.Flags = 0;

    IDXGISwapChain1* swap_chain1 = nullptr;
    d3dAssert(factory6->CreateSwapChainForHwnd(draw_command_queue_,
        window,
        &desc,
        nullptr,
        nullptr,
        &swap_chain1));

    IDXGISwapChain4* swap_chain = nullptr;
    d3dAssert(swap_chain1->QueryInterface(&swap_chain));
    swap_chain1->Release();

    factory6->Release();
    return swap_chain;
}

ID3D12DescriptorHeap* D3DRenderSystem::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, int num_descriptors)
{
    ID3D12DescriptorHeap* descriptor_heap;

    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = num_descriptors;
    desc.Type = type;

    d3dAssert(device_->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptor_heap)));

    return descriptor_heap;
}
