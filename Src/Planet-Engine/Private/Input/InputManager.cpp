#include "InputManager.h"
#include "PlanetLogging.h"

InputManager::InputManager()
{
    for (int i = 0; i < static_cast<int>(KeyCode::INVALID); ++i)
    {
        keys[i] = false;
    }
}

void InputManager::EndOfFrame()
{
    mouse_position_delta_.x_ = 0;
    mouse_position_delta_.y_ = 0;
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
    case WM_MOUSEMOVE:
    {
        int mouseX = GET_X_LPARAM(lParam);
        int mouseY = GET_Y_LPARAM(lParam);

        if (mouse_position_.x_ != -1)
        {
            mouse_position_delta_.x_ = mouseX - mouse_position_.x_;
            mouse_position_delta_.y_ = mouseY - mouse_position_.y_;
            P_LOG("Mouse move ({}, {})", mouse_position_delta_.x_, mouse_position_delta_.y_);
        }

        mouse_position_.x_ = mouseX;
        mouse_position_.y_ = mouseY;
        return true;
    }
    default:
        P_LOG("Not handling window msg 0x{:x} with params 0x{:x} 0x{:x}", msg, wParam, lParam);
        return false;
    }
}
#endif
