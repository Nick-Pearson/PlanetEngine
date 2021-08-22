#pragma once

#include <vector>

#include "Platform/Platform.h"
#include "Render/RenderSystem.h"
#include "Render/RenderQueue.h"
#include "Jobs/JobSystem.h"

class InputManager;
class RenderManager;

class PlanetEngine
{
 public:
    static PlanetEngine* Get();

    explicit PlanetEngine(RenderSystem* renderSystem);
    ~PlanetEngine();

    void Run();

    inline int GetExitCode() const { return exit_code_; }

    inline RenderQueue* GetRenderQueue() { return &render_queue_; }
    inline Renderer* GetRenderer() const { return render_system_->GetRenderer(); }
    inline RenderSystem* GetRenderSystem() const { return render_system_; }
    inline InputManager* GetInputManager() const { return input_manager_; }
    inline JobSystem* GetJobSystem() const { return job_system_; }

#if PLATFORM_WIN
    void PumpWindowsMessages();

    LRESULT CALLBACK ProcessWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    void RegisterMessageHandler(IWindowsMessageHandler* Handler);
    void UnregisterMessageHandler(IWindowsMessageHandler* Handler);
 private:
    std::vector<IWindowsMessageHandler*> message_handlers_;
#endif

 private:
    void SaveScreenshot(const CameraComponent& camera);

    int exit_code_;

    RenderQueue render_queue_;
    RenderSystem* render_system_;
    InputManager* input_manager_;
    JobSystem* job_system_;
};