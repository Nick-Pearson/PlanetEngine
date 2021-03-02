#pragma once

#include "Platform/PlanetWindows.h"
#include "D3DRenderer.h"

class D3DWindowEvents : public IWindowsMessageHandler
{
 public:
    explicit D3DWindowEvents(D3DRenderer* renderer);

    bool HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

 private:
    D3DRenderer* mRenderer;
};
