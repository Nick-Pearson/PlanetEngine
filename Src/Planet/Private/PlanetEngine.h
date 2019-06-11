#pragma once

#include "Platform/PlanetWindows.h"
#include "Renderer/Renderer.h"

class PlanetEngine
{
protected:
	PlanetEngine();
	~PlanetEngine();

public:

	static PlanetEngine* Get();

	void Run();

	inline int GetExitCode() const { return ExitCode; }

	inline Renderer* GetRenderer() const { return renderer; }
	
#if PLATFORM_WIN
	void PumpWindowsMessages();

	LRESULT CALLBACK ProcessWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

private:

	int ExitCode;

	Renderer* renderer;

};