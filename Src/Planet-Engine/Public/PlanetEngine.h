#pragma once

#include <vector>

#include "Platform/Platform.h"
#include "Render/RenderSystem.h"
#include "Render/RenderQueue.h"

class InputManager;
class RenderManager;

class PlanetEngine
{
 public:
    static PlanetEngine* Get();

    explicit PlanetEngine(RenderSystem* renderSystem);
    ~PlanetEngine();

    void Run();

    inline int GetExitCode() const { return ExitCode; }

    inline RenderQueue* GetRenderQueue() { return &render_queue_; }
    inline Renderer* GetRenderer() { return mRenderSystem->GetRenderer(); }
    inline RenderSystem* GetRenderSystem() { return mRenderSystem; }
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

    RenderQueue render_queue_;
    RenderSystem* mRenderSystem;
    InputManager* inputManager;
};