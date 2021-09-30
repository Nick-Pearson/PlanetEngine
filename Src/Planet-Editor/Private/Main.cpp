#include "PlanetEngine.h"
#include "Platform/PlanetWindows.h"
#include "PlanetEditor.h"
#include "Platform/Window.h"

#if PLATFORM_WIN
#include "D3DRenderSystem.h"
#include <shellscalingapi.h>

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
    Window window{1280, 720};
    window.SetWindowName("PlanetEngine");
    D3DRenderSystem* renderSystem = new D3DRenderSystem{window.GetWindowHandle()};
    PlanetEngine* eng = new PlanetEngine{renderSystem};

    const char* path = std::getenv("PROJECT_DIR");
    P_ASSERT(path != nullptr, "Project path undefined");
    PlanetEditor* editor = new PlanetEditor{eng, path};

    eng->Run();
    int code = eng->GetExitCode();

    // eng->Shutdown();
    delete editor;
    delete eng;
    delete renderSystem;

    return code;
}
#endif