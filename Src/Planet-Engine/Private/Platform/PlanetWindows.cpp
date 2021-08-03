#include "Platform/PlanetWindows.h"

KeyCode Platform::GetKeyCode(unsigned char VirtualKey)
{
    if (VirtualKey >= 'A' && VirtualKey <= 'Z')
    {
        return (KeyCode)(static_cast<int>(KeyCode::A) + (VirtualKey - 'A'));
    }
    else if (VirtualKey >= '0' && VirtualKey <= '9')
    {
        return (KeyCode)(static_cast<int>(KeyCode::ZERO) + (VirtualKey - '0'));
    }

    switch (VirtualKey)
    {
    case VK_LBUTTON:
        return KeyCode::LEFT_MOUSE_BUTTON;
    case VK_RBUTTON:
        return KeyCode::RIGHT_MOUSE_BUTTON;
    case VK_MBUTTON:
        return KeyCode::MIDDLE_MOUSE_BUTTON;
    case VK_BACK:
        return KeyCode::BACKSPACE;
    case VK_TAB:
        return KeyCode::TAB;
    case VK_RETURN:
        return KeyCode::RETURN;
    case VK_LSHIFT:
    case VK_SHIFT:
        return KeyCode::LEFT_SHIFT;
    case VK_RSHIFT:
        return KeyCode::RIGHT_SHIFT;
    case VK_LCONTROL:
    case VK_CONTROL:
        return KeyCode::LEFT_CTRL;
    case VK_RCONTROL:
        return KeyCode::RIGHT_CTRL;
    case VK_MENU:
        return KeyCode::ALT;
    case VK_PAUSE:
        return KeyCode::PAUSE;
    case VK_CAPITAL:
        return KeyCode::CAPS_LOCK;
    case VK_ESCAPE:
        return KeyCode::ESC;
    case VK_SPACE:
        return KeyCode::SPACE;
    case VK_PRIOR:
        return KeyCode::PAGE_UP;
    case VK_NEXT:
        return KeyCode::PAGE_DOWN;
    case VK_END:
        return KeyCode::END;
    case VK_HOME:
        return KeyCode::HOME;
    case VK_LEFT:
        return KeyCode::LEFT_ARROW;
    case VK_UP:
        return KeyCode::UP_ARROW;
    case VK_RIGHT:
        return KeyCode::RIGHT_ARROW;
    case VK_DOWN:
        return KeyCode::DOWN_ARROW;
    case VK_INSERT:
        return KeyCode::INSERT;
    case VK_DELETE:
        return KeyCode::DEL;
    case VK_LWIN:
        return KeyCode::LEFT_SUPER;
    case VK_RWIN:
        return KeyCode::RIGHT_SUPER;
    }

    return KeyCode::INVALID;
}
