#include "InputManager.h"
#include "PlanetLogging.h"

InputManager::InputManager()
{
    for (int i = 0; i < static_cast<int>(KeyCode::INVALID); ++i)
    {
        keys[i] = false;
    }
}

#if PLATFORM_WIN
bool InputManager::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    {
        if (!(lParam & 0x40000000))
        {
            KeyCode key = Platform::GetKeyCode((unsigned char)wParam);

            if (key != KeyCode::INVALID)
            {
                keys[static_cast<int>(key)] = true;
            }
        }
        return true;
    }
    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
        KeyCode key = Platform::GetKeyCode((unsigned char)wParam);

        if (key != KeyCode::INVALID)
        {
            keys[static_cast<int>(key)] = false;
        }
        return true;
    }
    default:
        return false;
    }
}
#endif
