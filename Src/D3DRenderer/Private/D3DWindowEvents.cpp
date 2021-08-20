#include "D3DWindowEvents.h"

D3DWindowEvents::D3DWindowEvents(D3DRenderSystem* renderer) :
    mRenderer(renderer)
{
}

bool D3DWindowEvents::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_SIZE)
    {
        mRenderer->UpdateWindowSize();
        return true;
    }
    return false;
}