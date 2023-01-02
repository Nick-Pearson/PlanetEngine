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
#include "Descriptor/SRVHeap.h"
#include "D3DAssert.h"
#include "D3DCommandQueue.h"
#include "BaseRootSignature.h"
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



#if defined(DX_DEBUG)
    // https://devblogs.microsoft.com/pix/taking-a-capture/
    static std::wstring GetLatestWinPixGpuCapturerPath()
    {
        LPWSTR programFilesPath = nullptr;
        SHGetKnownFolderPath(FOLDERID_ProgramFiles, KF_FLAG_DEFAULT, NULL, &programFilesPath);

        std::wstring pixSearchPath = std::wstring(L"\\\\?\\") + programFilesPath + std::wstring(L"\\Microsoft PIX\\*");

        WIN32_FIND_DATAW findData;
        bool foundPixInstallation = false;
        wchar_t newestVersionFound[MAX_PATH];

        HANDLE hFind = FindFirstFileW(pixSearchPath.c_str(), &findData);
        if (hFind != INVALID_HANDLE_VALUE)
        {
            for (int i = 0; i < 1000000; ++i)
            {
                if (((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) &&
                    (findData.cFileName[0] != '.'))
                {
                    if (!foundPixInstallation || wcscmp(newestVersionFound, findData.cFileName) <= 0)
                    {
                        foundPixInstallation = true;
                        StringCchCopyW(newestVersionFound, _countof(newestVersionFound), findData.cFileName);
                    }
                }
                if (!FindNextFileW(hFind, &findData))
                {
                    break;
                }
            }
        }

        FindClose(hFind);
        P_ASSERT(foundPixInstallation, "PIX dll not found, unable to load PIX debugging");

        wchar_t output[MAX_PATH];
        StringCchCopyW(output, pixSearchPath.length(), pixSearchPath.data());
        StringCchCatW(output, MAX_PATH, &newestVersionFound[0]);
        StringCchCatW(output, MAX_PATH, L"\\WinPixGpuCapturer.dll");

        return &output[0];
    }
#endif
}  // namespace

D3DRenderSystem::D3DRenderSystem(HWND window)
{
#if defined(DX_DEBUG)
    {
        d3dAssert(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_)));
        debug_->EnableDebugLayer();

        if (GetModuleHandleW(L"WinPixGpuCapturer.dll") == 0)
        {
            LoadLibraryW(GetLatestWinPixGpuCapturerPath().c_str());
        }
    }
#endif

    InitDevice(window);

    draw_command_allocator_->Reset();
    d3dAssert(draw_command_list_->Reset(draw_command_allocator_, nullptr));

    // Compile Shaders
    const D3DVertexShader* vertex_shader = D3DShaderLoader::LoadVertex("VertexShader.hlsl");
    root_signature_ = new BaseRootSignature{ device_ };
    root_signature_->Bind(draw_command_list_);

    ui_renderer_ = new ImGUIRenderer{ window, device_, srv_heap_, draw_command_list_ };
    resource_manager_ = new GPUResourceManager{ device_, srv_heap_ };
    window_events_ = new D3DWindowEvents{ this };

    // Material wireframe_material{"WireframeShader.hlsl"};
    // auto wireframe_shader = mResourceManager->LoadMaterial(&wireframe_material);
    renderer_ = new D3DRenderer{ device_, draw_command_list_, root_signature_, srv_heap_ };
    window_render_target_ = new WindowRenderTarget{device_, swap_chain_, rtv_descriptor_heap_, dsv_descriptor_heap_, draw_command_queue_};
    renderer_->BindRenderTarget(window_render_target_);

#if defined(DX_DEBUG)
    FlushDebugMessages();
#endif

    draw_command_queue_->ExecuteCommandList(draw_command_list_);
    auto signal = draw_command_queue_->Signal();
    draw_command_queue_->WaitForSignal(signal);

    last_present_time_ = std::chrono::steady_clock::now();
}

D3DRenderSystem::~D3DRenderSystem()
{
    delete renderer_;
    delete ui_renderer_;
    delete resource_manager_;
    delete window_events_;
    delete window_render_target_;

    delete draw_command_queue_;
    draw_command_allocator_->Release();
    draw_command_list_->Release();
    delete compute_command_queue_;

    rtv_descriptor_heap_->Release();
    dsv_descriptor_heap_->Release();

    delete root_signature_;

    device_->Release();
    adapter_->Release();
}

void D3DRenderSystem::Load(class PlanetEngine* engine)
{
    engine->RegisterMessageHandler(window_events_);
    ui_renderer_->NewFrame();

    engine->GetJobSystem()->RunJobRepeatedly([=](){ resource_manager_->ProcessCompletedBatches(); }, 500);
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
        auto d3d_mesh = resource_manager_->LoadMesh(mesh->GetMesh());
        auto d3d_material = resource_manager_->LoadMaterial(mesh->GetMaterial());

        RenderState render_state{
            d3d_mesh,
            d3d_material,
            mesh->GetWorldTransform(),
            mesh->render_config_.use_depth_buffer_,
            mesh->render_config_.use_world_matrix_
        };

        renderer_->AddRenderState(render_state);
    }

    resource_manager_->ExecuteResourceLoads();
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
    renderer_->UnbindRenderTarget();
    window_render_target_->UpdateWindowSize(device_);
    renderer_->BindRenderTarget(window_render_target_);
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
    window_render_target_->PreRender();
    renderer_->Render(camera);
    RenderDebugUI();
    ui_renderer_->Render();

    Present();
    ui_renderer_->NewFrame();
    FlushDebugMessages();
}

void D3DRenderSystem::Present()
{
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(window_render_target_->GetRenderTarget()->resource_,
        D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    draw_command_list_->ResourceBarrier(1, &barrier);

    draw_command_queue_->ExecuteCommandList(draw_command_list_);

    window_render_target_->Present();

    const auto present_time = std::chrono::steady_clock::now();
    const auto frame_time = (present_time - last_present_time_)/std::chrono::milliseconds(1);
    frame_times_ms_.Add(frame_time);

    last_present_time_ = present_time;
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
    ImGui::Begin("Rendering");
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    BOOL fullscreen;
    d3dAssert(swap_chain_->GetDesc(&swapChainDesc));
    d3dAssert(swap_chain_->GetFullscreenState(&fullscreen, nullptr));

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

    // mResourceManager->RenderDebugUI();
    // mRenderer->RenderDebugUI();

    ImGui::End();
}

void D3DRenderSystem::InitDevice(HWND window)
{
    device_ = CreateDevice();

    draw_command_queue_ = new D3DCommandQueue{device_, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_QUEUE_PRIORITY_HIGH};
    SET_NAME(draw_command_queue_, "Draw Command Queue")
    d3dAssert(device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&draw_command_allocator_)));
    SET_NAME(draw_command_allocator_, "Draw Command Allocator")
    d3dAssert(device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, draw_command_allocator_, nullptr, IID_PPV_ARGS(&draw_command_list_)));
    SET_NAME(draw_command_list_, "Draw Command List")
    d3dAssert(draw_command_list_->Close());

    compute_command_queue_ = new D3DCommandQueue{device_, D3D12_COMMAND_LIST_TYPE_COMPUTE, D3D12_COMMAND_QUEUE_PRIORITY_NORMAL};
    SET_NAME(draw_command_queue_, "Compute Command Queue")

    swap_chain_ = CreateSwapChain(window);

    rtv_descriptor_heap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, NUM_BUFFERS);
    dsv_descriptor_heap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);

    srv_heap_ = new SRVHeap{device_};
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
    desc.SampleDesc = DXGI_SAMPLE_DESC{ 1, 0 };
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 3;
    desc.Scaling = DXGI_SCALING_STRETCH;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    desc.Flags = 0;

    IDXGISwapChain1* swap_chain1 = nullptr;
    d3dAssert(factory6->CreateSwapChainForHwnd(draw_command_queue_->GetQueue(),
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
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    d3dAssert(device_->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptor_heap)));

    return descriptor_heap;
}
