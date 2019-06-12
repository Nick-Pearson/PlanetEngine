#pragma once

#ifdef WINVER
#undef WINVER
#endif

#define WIN32_LEAN_AND_MEAN
#define WINVER 0x0500

#include <Windows.h>
#include "../Input/KeyCode.h"

class IWindowsMessageHandler
{
public:
	virtual bool HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;
};

namespace Platform
{
	extern KeyCode GetKeyCode(unsigned char VirtualKey);
}