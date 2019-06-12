#include "PlanetEngine.h"

#include "Platform/Window.h"
#include "Mesh/Mesh.h"

#include "Mesh/OBJImporter.h"
#include "Mesh/Primitives.h"
#include "Entity/Entity.h"
#include "World/Scene.h"
#include "Mesh/MeshComponent.h"
#include "World/CameraComponent.h"
#include "World/SkyDome.h"
#include "Input/InputManager.h"
#include "Editor/FlyCam.h"

#include <memory>
#include <chrono>
#include <thread>

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

	inputManager = new InputManager{};
	RegisterMessageHandler(inputManager);

	std::shared_ptr<Mesh> bunny = OBJImporter::Import("Assets/Models/bunny.obj", 20.0f);

	std::shared_ptr<Scene> scene = std::make_shared<Scene>();

	std::shared_ptr<FlyCam> cameraEntity = scene->SpawnEntity<FlyCam>();
	std::shared_ptr<CameraComponent> cameraComp = cameraEntity->GetCamera();
	cameraEntity->Translate(Vector{ 0.0f, 2.0f, -4.0f });

	scene->SpawnEntity()->AddComponent<MeshComponent>(bunny, "PixelShader.hlsl");
	scene->SpawnEntity<SkyDome>();

	float deltaTime = 0.01f;
	auto begin = std::chrono::high_resolution_clock::now();

	ExitCode = -1;
	while (ExitCode == -1)
	{
#if PLATFORM_WIN
		PumpWindowsMessages();
#endif
		//ProcessInput();

		scene->Update(deltaTime);
		
		//renderer.RenderMesh(cube);
		renderer->Render(cameraComp);
		renderer->SwapBuffers();

		auto end = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(end - begin).count();
		begin = end;
	}

	delete renderer;
	UnregisterMessageHandler(inputManager);
	delete inputManager;
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

	for (IWindowsMessageHandler* handler : messageHandlers)
	{
		if (handler->HandleMessage(hWnd, msg, wParam, lParam))
			return true;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void PlanetEngine::RegisterMessageHandler(IWindowsMessageHandler* Handler)
{
	messageHandlers.push_back(Handler);
}

void PlanetEngine::UnregisterMessageHandler(IWindowsMessageHandler* Handler)
{
	auto it = messageHandlers.begin();
	for (; it != messageHandlers.end(); it++)
	{
		if ((*it) == Handler) break;
	}

	if(it != messageHandlers.end())
		messageHandlers.erase(it);
}

#endif