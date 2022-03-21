#pragma once

#include <vector>
#include <string>
#include <functional>

#ifdef WINVER
#undef WINVER
#endif

#define WIN32_LEAN_AND_MEAN
#define WINVER 0x0500

#include <Windows.h>
#include <windowsx.h>
#include <shlobj.h>
#include <strsafe.h>
#include "../Input/KeyCode.h"

class IWindowsMessageHandler
{
 public:
    virtual bool HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;
};

namespace Platform
{
    extern KeyCode GetKeyCode(unsigned char VirtualKey);

    extern void CreateDirectoryIfNotExists(const char* directory);

    extern void AddDirectoryChangeListener(const char* directory, const std::function<void()>& change_func);

    extern std::vector<std::string> ListFiles(const char* directory);
    extern std::vector<std::string> ListDirectories(const char* directory);
}