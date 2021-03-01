#pragma once

#include "Platform/PlanetWindows.h"
#include "imconfig.h"
#include "imgui.h"

class ImGuiInput : public IWindowsMessageHandler
{
 public:
    bool HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;
 private:
    static bool ImGui_ImplWin32_UpdateMouseCursor();
};
