#include "D3DWindowEvents.h"

D3DWindowEvents::D3DWindowEvents(D3DRenderSystem* renderer) :
    renderer_(renderer)
{
}

bool D3DWindowEvents::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_SIZE)
    {
        renderer_->UpdateWindowSize();
        return true;
    }
    return false;
}