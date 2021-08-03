#include "PlanetEngine.h"

#include <assert.h>
#include <memory>
#include <chrono>
#include <thread>

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
#include "Input/ImGuiInput.h"
#include "Editor/FlyCam.h"
#include "Material/Material.h"
#include "Texture/Texture2D.h"
#include "Texture/TextureFactory.h"
#include "Compute/ComputeShader.h"

namespace
{
    PlanetEngine* sEngine;
}  // namespace

PlanetEngine::PlanetEngine(RenderSystem* renderSystem)
{
    assert(sEngine == nullptr);
    sEngine = this;
    mRenderSystem = renderSystem;
}

PlanetEngine::~PlanetEngine()
{
    sEngine = nullptr;
}

// TODO: Remove this global
PlanetEngine* PlanetEngine::Get()
{
    return sEngine;
}

class CameraComponent;

void PlanetEngine::Run()
{
    inputManager = new InputManager{};
    RegisterMessageHandler(inputManager);
    ImGuiInput* imguiInput = new ImGuiInput{};
    RegisterMessageHandler(imguiInput);
    mRenderSystem->Load(this);

    std::shared_ptr<Mesh> bunny = OBJImporter::Import("Assets/Models/bunny.obj", 20.0f);

    std::shared_ptr<Scene> scene = std::make_shared<Scene>();

    std::shared_ptr<Material> standardMaterial = std::make_shared<Material>("PixelShader.hlsl");
    std::shared_ptr<Material> texturedMaterial = std::make_shared<Material>("TexturedShader.hlsl");

    std::shared_ptr<Texture2D> texture = TextureFactory::fromFile("Assets/Textures/JailFloor.png");
    texturedMaterial->AddTexture(texture);

    std::shared_ptr<FlyCam> cameraEntity = scene->SpawnEntity<FlyCam>("camera");
    cameraEntity->Translate(Vector{ 0.0f, 4.0f, 10.0f });
    cameraEntity->Rotate(Vector{ 0.0f, 180.0f, 0.0f });

    std::shared_ptr<Entity> bunnyEntity = scene->SpawnEntity("bunny");
    bunnyEntity->AddComponent<MeshComponent>(bunny, standardMaterial);
    bunnyEntity->Translate(Vector{ 0.0f, 0.6f, 5.0f });

    std::shared_ptr<Entity> floorEntity = scene->SpawnEntity("floor");
    floorEntity->Rotate(Vector{ 90.0f, 90.0f, -90.0f });
    floorEntity->AddComponent<MeshComponent>(Primitives::Plane(100000.0f), standardMaterial);

    std::shared_ptr<Entity> planeEntity = scene->SpawnEntity("wall");
    planeEntity->Rotate(Vector{ 0.0f, 160.0f, 0.0f });
    planeEntity->Translate(Vector{ -4.0f, -2.0f, -2.0f });
    planeEntity->AddComponent<MeshComponent>(Primitives::Plane(2.0f), texturedMaterial);

    scene->SpawnEntity<SkyDome>("sky");

    float deltaTime = 0.01f;
    auto begin = std::chrono::high_resolution_clock::now();

    const CameraComponent& cameraComp = cameraEntity->GetCamera();

    ExitCode = -1;
    while (ExitCode == -1)
    {
#if PLATFORM_WIN
        PumpWindowsMessages();
#endif
        // ProcessInput();

        scene->Update(deltaTime);

        mRenderSystem->ApplyQueue(render_queue_.GetItems());
        render_queue_.ClearQueue();
        mRenderSystem->RenderFrame(cameraComp);
        inputManager->EndOfFrame();

        auto end = std::chrono::high_resolution_clock::now();
        deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(end - begin).count();
        begin = end;
    }

    mRenderSystem->UnLoad(this);
    UnregisterMessageHandler(inputManager);
    delete imguiInput;
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

    if (it != messageHandlers.end())
        messageHandlers.erase(it);
}

#endif