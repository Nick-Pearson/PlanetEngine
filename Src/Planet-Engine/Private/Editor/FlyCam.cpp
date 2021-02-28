#include "FlyCam.h"
#include "PlanetEngine.h"
#include "../Input/InputManager.h"
#include "../World/CameraComponent.h"
#include "imgui.h"

FlyCam::FlyCam()
{
    camera = AddComponent<CameraComponent>();
}

void FlyCam::OnUpdate(float deltaSeconds)
{
    InputManager* mgr = PlanetEngine::Get()->GetInputManager();

    if (!mgr) return;

    Vector movement, rotation;

    if (mgr->GetIsKeyDown(KeyCode::W))
    {
        movement += Vector(0.0f, 0.0f, 1.0f);
    }
    else if (mgr->GetIsKeyDown(KeyCode::S))
    {
        movement += Vector(0.0f, 0.0f, -1.0f);
    }

    if (mgr->GetIsKeyDown(KeyCode::D))
    {
        movement += Vector(1.0f, 0.0f, 0.0f);
    }
    else if (mgr->GetIsKeyDown(KeyCode::A))
    {
        movement += Vector(-1.0f, 0.0f, 0.0f);
    }

    if (mgr->GetIsKeyDown(KeyCode::UP_ARROW))
    {
        rotation += Vector(-1.0f, 0.0f, 0.0f);
    }
    else if (mgr->GetIsKeyDown(KeyCode::DOWN_ARROW))
    {
        rotation += Vector(1.0f, 0.0f, 0.0f);
    }

    if (mgr->GetIsKeyDown(KeyCode::LEFT_ARROW))
    {
        rotation += Vector(0.0f, -1.0f, 0.0f);
    }
    else if (mgr->GetIsKeyDown(KeyCode::RIGHT_ARROW))
    {
        rotation += Vector(0.0f, 1.0f, 0.0f);
    }

    Rotate(rotation * deltaSeconds * mTurnSpeed);
    Translate((transform.rotation * movement) * deltaSeconds * mMoveSpeed);

    ImGui::Begin("Camera");
    if (ImGui::Button("Reset Camera"))
    {
        transform = Transform();
        Translate(Vector{ 0.0f, 4.0f, 10.0f });
        Rotate(Vector{ 0.0f, 180.0f, 0.0f });
    }
    ImGui::End();
}
