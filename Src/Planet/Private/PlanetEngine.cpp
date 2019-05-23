#include "PlanetEngine.h"

#include "Platform/Window.h"
#include "Renderer/Renderer.h"

PlanetEngine::PlanetEngine()
{
}

PlanetEngine::~PlanetEngine()
{

}

PlanetEngine* PlanetEngine::Get()
{
	static PlanetEngine* sEngine = nullptr;

	if (!sEngine) sEngine = new PlanetEngine{};
	return sEngine;
}

void PlanetEngine::Run()
{
	Window window{1280, 720};
	window.SetWindowName("PlanetEngine");

	Renderer renderer{ window };

	ExitCode = -1;
	while (ExitCode == -1)
	{
#if PLATFORM_WIN
		PumpWindowsMessages();
#endif
		//ProcessInput();
		//UpdateGameplay();
		
		renderer.Render();
		renderer.SwapBuffers();
	}
}

#if PLATFORM_WIN
void PlanetEngine::PumpWindowsMessages()
{
	MSG msg;
	BOOL result;

	while ((result = PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) != 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

LRESULT CALLBACK PlanetEngine::ProcessWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		ExitCode = 0;
		return 0;
	case WM_QUIT:
		ExitCode = wParam;
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
#endif