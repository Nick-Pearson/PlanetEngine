#pragma once

#include "Platform.h"

class Window
{
 public:
    Window(int sizeX, int sizeY);
    ~Window();

    void SetWindowName(const char* WindowName);

#if PLATFORM_WIN
    inline HWND GetWindowHandle() const { return mWindowHandle; }
#endif

    inline int GetSizeX() const { return width; }
    inline int GetSizeY() const { return height; }

 private:
    class PlanetEngine* enginePtr;

#if PLATFORM_WIN
    static LRESULT CALLBACK SetupWindowMessages(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK sProcessWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT CALLBACK ProcessWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    HWND mWindowHandle;
#endif

    int width;
    int height;
};