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
#include "Renderer/RenderManager.h"

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

	mRenderManager = new RenderManager{ &window };

	inputManager = new InputManager{};
	RegisterMessageHandler(inputManager);

	std::shared_ptr<Mesh> bunny = OBJImporter::Import("Assets/Models/bunny.obj", 20.0f);

	std::shared_ptr<Scene> scene = std::make_shared<Scene>();

	std::shared_ptr<FlyCam> cameraEntity = scene->SpawnEntity<FlyCam>("camera");
	std::shared_ptr<CameraComponent> cameraComp = cameraEntity->GetCamera();
	cameraEntity->Translate(Vector{ 0.0f, 4.0f, 10.0f });
	cameraEntity->Rotate(Vector{ 15.0f, 180.0f, 0.0f });
	mRenderManager->SetCamera(cameraComp);

	std::shared_ptr<Entity> bunnyEntity = scene->SpawnEntity("bunny");
	bunnyEntity->AddComponent<MeshComponent>(bunny, "PixelShader.hlsl");
	bunnyEntity->Translate(Vector{ 0.0f, 0.6f, 0.0f });
	
	std::shared_ptr<Entity> floorEntity = scene->SpawnEntity("floor");
	floorEntity->Rotate(Vector{ 90.0f, 90.0f, 0.0f });
	floorEntity->AddComponent<MeshComponent>(Primitives::Plane(10.0f), "PixelShader.hlsl");
	floorEntity->Rotate(Vector{ 0.0f, 0.0f, -90.0f});

	scene->SpawnEntity<SkyDome>("sky");

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
		
		mRenderManager->RenderFrame();

		auto end = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(end - begin).count();
		begin = end;
	}

	delete mRenderManager;
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