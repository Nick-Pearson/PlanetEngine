#pragma once

#include "../Platform/Platform.h"
#include "KeyCode.h"

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

private:

	bool keys[(int)KeyCode::INVALID];

};