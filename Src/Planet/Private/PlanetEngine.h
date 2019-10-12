#pragma once

#include "Platform/Platform.h"

#include <vector>

class InputManager;
class RenderManager;

class PlanetEngine
{
protected:
	PlanetEngine();
	~PlanetEngine();

public:

	static PlanetEngine* Get();

	void Run();

	inline int GetExitCode() const { return ExitCode; }

	inline RenderManager* GetRenderManager() const { return mRenderManager; }
	inline InputManager* GetInputManager() const { return inputManager; }

#if PLATFORM_WIN
	void PumpWindowsMessages();

	LRESULT CALLBACK ProcessWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void RegisterMessageHandler(IWindowsMessageHandler* Handler);
	void UnregisterMessageHandler(IWindowsMessageHandler* Handler);
private:

	std::vector<IWindowsMessageHandler*> messageHandlers;
#endif

private:

	int ExitCode;

	RenderManager* mRenderManager;
	InputManager* inputManager;

};