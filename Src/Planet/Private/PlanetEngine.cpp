#include "PlanetEngine.h"

#include "Platform/Window.h"
#include "Mesh/Mesh.h"

#include <memory>
#include "Mesh/OBJImporter.h"
#include "Mesh/Primitives.h"
#include "Entity/Entity.h"
#include "World/Scene.h"
#include "Mesh/MeshComponent.h"
#include "World/CameraComponent.h"
#include "World/SkyDome.h"

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

class CameraComponent;

void PlanetEngine::Run()
{
	Window window{1280, 720};
	window.SetWindowName("PlanetEngine");

	renderer = new Renderer{ window };

	std::shared_ptr<Mesh> bunny = OBJImporter::Import("Assets/Models/bunny.obj", 20.0f);

	std::shared_ptr<Scene> scene = std::make_shared<Scene>();

	std::shared_ptr<Entity> cameraEntity = scene->SpawnEntity();
	std::shared_ptr<CameraComponent> cameraComp = cameraEntity->AddComponent<CameraComponent>();
	cameraEntity->Translate(Vector{ 0.0f, 0.0f, -4.0f });

	scene->SpawnEntity()->AddComponent<MeshComponent>(bunny, "PixelShader.hlsl");
	scene->SpawnEntity<SkyDome>();

	ExitCode = -1;
	while (ExitCode == -1)
	{
#if PLATFORM_WIN
		PumpWindowsMessages();
#endif
		//ProcessInput();

		//scene->Update(deltaSeconds);
		
		//renderer.RenderMesh(cube);
		renderer->Render(cameraComp);
		renderer->SwapBuffers();
	}

	delete renderer;
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