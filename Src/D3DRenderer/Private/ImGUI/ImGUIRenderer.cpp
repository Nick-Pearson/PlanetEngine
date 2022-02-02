#include "ImGUIRenderer.h"

#include "./planet_imgui.h"

#include "imgui.h"
// #include "imgui_impl_dx11.h"

ImGUIRenderer::ImGUIRenderer(HWND window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(window);
    // ImGui_ImplDX11_Init(device.Get(), context.Get());
}

ImGUIRenderer::~ImGUIRenderer()
{
    ImGui_ImplWin32_Shutdown();
    // ImGui_ImplDX11_Shutdown();
    ImGui::DestroyContext();
}

void ImGUIRenderer::Render()
{
    ImGui::Render();
    // ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGUIRenderer::NewFrame()
{
    // ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}