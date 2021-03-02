#include "D3DWindowEvents.h"

D3DWindowEvents::D3DWindowEvents(D3DRenderer* renderer) :
    mRenderer(renderer)
{
}

bool D3DWindowEvents::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_SIZE)
    {
        mRenderer->UpdateWindowSize(true);
        return true;
    }
    return false;
}