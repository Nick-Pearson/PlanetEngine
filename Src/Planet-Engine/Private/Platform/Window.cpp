#include "Window.h"

#include "PlanetEngine.h"
#include "imgui.h"
#include "planet_imgui.h"

namespace
{
}

Window::Window(int sizeX, int sizeY) : 
	width(sizeX), height(sizeY)
{
	enginePtr = PlanetEngine::Get();

#if PLATFORM_WIN
	const auto ClassName = "PlanetEngWindowClass";

	HINSTANCE hInstance = GetModuleHandle(NULL);

	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = Window::SetupWindowMessages;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = ClassName;
	wc.hIconSm = nullptr;

	RegisterClassEx(&wc);

	const DWORD WindowStyles = WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;
	mWindowHandle = CreateWindowEx(0, ClassName, "", WindowStyles, 200, 200, sizeX, sizeY, nullptr, nullptr, hInstance, (LPVOID)this);
	ShowWindow(mWindowHandle, SW_SHOW);
#else
	#error "Window construction not implemented on this platform"
#endif
}

Window::~Window()
{
#if PLATFORM_WIN
	DestroyWindow(mWindowHandle);
#else
#error "Window destruction not implemented on this platform"
#endif
}

void Window::SetWindowName(const char* WindowName)
{

}

#if PLATFORM_WIN
LRESULT CALLBACK Window::SetupWindowMessages(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_NCCREATE)
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const windowPtr = static_cast<Window*>(pCreate->lpCreateParams);

		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(windowPtr));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::sProcessWindowMessage));

		return windowPtr->ProcessWindowMessage(hWnd, msg, wParam, lParam);
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::sProcessWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window* const windowPtr = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return windowPtr->ProcessWindowMessage(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::ProcessWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// do some window stuff
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}

	return enginePtr->ProcessWindowMessage(hWnd, msg, wParam, lParam);
}
#endif
