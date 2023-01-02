#include "ImGUIRenderer.h"

#include "./planet_imgui.h"

#include "imgui.h"
#include "imgui_impl_dx12.h"

ImGUIRenderer::ImGUIRenderer(HWND window, ID3D12Device2* device, SRVHeap* srv_heap, ID3D12GraphicsCommandList* command_list) :
    command_list_(command_list)
{
    command_list_->AddRef();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    auto font_handle = srv_heap->CreateDescriptorHandle();
    auto heap = srv_heap->GetDescriptorHeap();

    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX12_Init(device, 3, DXGI_FORMAT_R8G8B8A8_UNORM, heap, font_handle.cpu_, font_handle.gpu_);
}

ImGUIRenderer::~ImGUIRenderer()
{
    ImGui_ImplWin32_Shutdown();
    ImGui_ImplDX12_Shutdown();
    ImGui::DestroyContext();
    command_list_->Release();
}

void ImGUIRenderer::Render()
{
    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), command_list_);
}

void ImGUIRenderer::NewFrame()
{
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}