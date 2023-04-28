#pragma once

#include "Platform/PlanetWindows.h"
#include "D3DRenderSystem.h"

class D3DWindowEvents : public IWindowsMessageHandler
{
 public:
    explicit D3DWindowEvents(D3DRenderSystem* renderer);

    bool HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

 private:
    D3DRenderSystem* renderer_;
};
