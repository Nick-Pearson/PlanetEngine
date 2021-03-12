#pragma once

#include "Platform/Platform.h"
#include "Input/KeyCode.h"

class InputManager
#if PLATFORM_WIN
    : public IWindowsMessageHandler
#endif
{
 public:
    InputManager();

#if PLATFORM_WIN
    bool HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;
#endif

    inline bool GetIsKeyDown(KeyCode key) const
    {
        if (key == KeyCode::INVALID) return false;
        return keys[static_cast<int>(key)];
    }

 private:
    bool keys[static_cast<int>(KeyCode::INVALID)];
};