#pragma once

#include "Platform/Platform.h"
#include "Renderer/Renderer.h"

class InputManager;

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

	Renderer* renderer;
	InputManager* inputManager;

};