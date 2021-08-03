#pragma once

#include "Platform/Platform.h"
#include "Input/KeyCode.h"
#include "Math/iVec2D.h"

class InputManager
#if PLATFORM_WIN
    : public IWindowsMessageHandler
#endif
{
 public:
    InputManager();

    void EndOfFrame();

#if PLATFORM_WIN
    bool HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;
#endif

    inline bool GetIsKeyDown(KeyCode key) const
    {
        if (key == KeyCode::INVALID) return false;
        return keys[static_cast<int>(key)];
    }

    inline iVec2D GetMouseDelta() const
    {
        return mouse_position_delta_;
    }

    inline iVec2D GetMousePosition() const
    {
        return mouse_position_;
    }

 private:
    bool keys[static_cast<int>(KeyCode::INVALID)];
    iVec2D mouse_position_ = iVec2D{-1, -1};
    iVec2D mouse_position_delta_ = iVec2D{0, 0};
};