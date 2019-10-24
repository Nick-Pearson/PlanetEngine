#include "UIRenderer.h"

#include "../Platform/Window.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

UIRenderer::UIRenderer(const Window* window, Microsoft::WRL::ComPtr <ID3D11Device> device, Microsoft::WRL::ComPtr <ID3D11DeviceContext> context)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window->GetWindowHandle());
	ImGui_ImplDX11_Init(device.Get(), context.Get());
	NewFrame();
}

UIRenderer::~UIRenderer()
{
	ImGui_ImplWin32_Shutdown();
	ImGui_ImplDX11_Shutdown();
	ImGui::DestroyContext();
}

void UIRenderer::Render()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void UIRenderer::NewFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}
