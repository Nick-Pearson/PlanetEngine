#include "PlanetEngine.h"

#include <assert.h>
#include <memory>
#include <chrono>
#include <thread>

#include "Platform/Window.h"
#include "Mesh/Mesh.h"

#include "Mesh/OBJImporter.h"
#include "Mesh/FBXImporter.h"
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
#include "Texture/TextureWriter.h"
#include "Compute/ComputeShader.h"
#include "Jobs/ThreadPoolJobSystem.h"

namespace
{
    PlanetEngine* sEngine;
}  // namespace

PlanetEngine::PlanetEngine(RenderSystem* renderSystem)
{
    assert(sEngine == nullptr);
    sEngine = this;
    PlanetLogging::init_logging();
    render_system_ = renderSystem;
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
namespace chr = std::chrono;

void PlanetEngine::Run()
{
    input_manager_ = new InputManager{};
    RegisterMessageHandler(input_manager_);
    ImGuiInput* imguiInput = new ImGuiInput{};
    RegisterMessageHandler(imguiInput);
    render_system_->Load(this);
    job_system_ = new ThreadPoolJobSystem{2};

    std::shared_ptr<Mesh> bunny = OBJImporter::Import("Assets/Models/bunny.obj", 20.0f);
    std::shared_ptr<Mesh> tree = FBXImporter::Import("Assets/Models/tree/Aset_wood_root_M_rkswd_LOD0.fbx", 1.0f);
    tree->RecalculateNormals();

    std::shared_ptr<Scene> scene = std::make_shared<Scene>();

    std::shared_ptr<Material> standardMaterial = std::make_shared<Material>("PixelShader.hlsl");
    std::shared_ptr<Material> texturedMaterial = std::make_shared<Material>("TexturedShader.hlsl");

    std::shared_ptr<Texture2D> texture = TextureFactory::fromFile("Assets/Textures/JailFloor.png");
    texturedMaterial->AddTexture(texture);

    std::shared_ptr<FlyCam> cameraEntity = scene->SpawnEntity<FlyCam>("camera");
    cameraEntity->Translate(Vector{ 0.0f, 4.0f, 10.0f });
    cameraEntity->Rotate(Vector{ 0.0f, 180.0f, 0.0f });

    std::shared_ptr<Entity> bunnyEntity = scene->SpawnEntity("bunny");
    bunnyEntity->AddComponent<MeshComponent>(tree, standardMaterial);
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

    exit_code_ = -1;
    while (exit_code_ == -1)
    {
#if PLATFORM_WIN
        PumpWindowsMessages();
#endif
        // ProcessInput();

        scene->Update(deltaTime);

        render_system_->ApplyQueue(render_queue_.GetItems());
        render_queue_.ClearQueue();
        render_system_->RenderFrame(cameraComp);

        if (input_manager_->GetIsKeyDown(KeyCode::P))
        {
            SaveScreenshot(cameraComp);
        }

        input_manager_->EndOfFrame();

        auto end = std::chrono::high_resolution_clock::now();
        deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(end - begin).count();
        begin = end;
    }

    render_system_->UnLoad(this);
    UnregisterMessageHandler(input_manager_);
    delete imguiInput;
    delete input_manager_;
    delete job_system_;
}

void PlanetEngine::SaveScreenshot(const CameraComponent& camera)
{
    chr::high_resolution_clock::time_point start = chr::high_resolution_clock::now();

    Texture2D* texture = new Texture2D{3840, 2160};
    render_system_->RenderToTexture(texture, camera);

    bool added = job_system_->RunJob([=]() {
        Platform::CreateDirectoryIfNotExists("screenshots");
        TextureWriter::writeToFile("screenshots/screenshot.png", *texture);
        auto time = chr::high_resolution_clock::now() - start;
        P_LOG("Captured {}x{} screnshot in {}ms", texture->GetWidth(), texture->GetHeight(), time/chr::milliseconds(1));
    });

    if (!added)
    {
        delete texture;
        P_WARN("Failed to add screenshot job to job system");
    }
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
        exit_code_ = 0;
        return 0;
    case WM_QUIT:
        exit_code_ = wParam;
        break;
    }

    for (IWindowsMessageHandler* handler : message_handlers_)
    {
        if (handler->HandleMessage(hWnd, msg, wParam, lParam))
            return true;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void PlanetEngine::RegisterMessageHandler(IWindowsMessageHandler* Handler)
{
    message_handlers_.push_back(Handler);
}

void PlanetEngine::UnregisterMessageHandler(IWindowsMessageHandler* Handler)
{
    auto it = message_handlers_.begin();
    for (; it != message_handlers_.end(); it++)
    {
        if ((*it) == Handler) break;
    }

    if (it != message_handlers_.end())
        message_handlers_.erase(it);
}

#endif