#include "PlanetEngine.h"
#include "Platform/PlanetWindows.h"

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    PlanetEngine* eng = PlanetEngine::Get();

    eng->Run();
    int code = eng->GetExitCode();

    // eng->Shutdown();

    return code;
}