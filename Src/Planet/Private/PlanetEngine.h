#pragma once

#include "Platform/PlanetWindows.h"

class PlanetEngine
{
protected:
	PlanetEngine();
	~PlanetEngine();

public:

	static PlanetEngine* Get();

	void Run();

	inline int GetExitCode() const { return ExitCode; }
	
#if PLATFORM_WIN
	void PumpWindowsMessages();

	LRESULT CALLBACK ProcessWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

private:

	int ExitCode;

};