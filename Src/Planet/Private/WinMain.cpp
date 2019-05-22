#include "PlanetEngine.h"
#include "Platform/PlanetWindows.h"

/*
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
*/
int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	PlanetEngine* eng = PlanetEngine::Get();
	
	eng->Run();
	int code = eng->GetExitCode();

	//eng->Shutdown();

	return code;
}