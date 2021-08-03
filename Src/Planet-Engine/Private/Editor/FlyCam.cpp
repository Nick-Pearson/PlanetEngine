#include "FlyCam.h"
#include "PlanetEngine.h"
#include "../Input/InputManager.h"
#include "World/CameraComponent.h"
#include "imgui.h"

FlyCam::FlyCam()
{
    camera = AddComponent<CameraComponent>();
}

void FlyCam::OnUpdate(float deltaSeconds)
{
    InputManager* mgr = PlanetEngine::Get()->GetInputManager();

    if (!mgr) return;

    Vector movement;
    float rotation_x = 0.0f;
    float rotation_y = 0.0f;

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

    if (mgr->GetIsKeyDown(KeyCode::LEFT_SHIFT))
    {
        movement *= 10.0f;
    }

    if (mgr->GetIsKeyDown(KeyCode::UP_ARROW))
    {
        rotation_y -= 1.0f;
    }
    else if (mgr->GetIsKeyDown(KeyCode::DOWN_ARROW))
    {
        rotation_y += 1.0f;
    }

    if (mgr->GetIsKeyDown(KeyCode::LEFT_ARROW))
    {
        rotation_x -= 1.0f;
    }
    else if (mgr->GetIsKeyDown(KeyCode::RIGHT_ARROW))
    {
        rotation_x += 1.0f;
    }

    if (mgr->GetIsKeyDown(KeyCode::LEFT_CTRL))
    {
        iVec2D mouse_delta = mgr->GetMouseDelta();
        rotation_x += mouse_sensitivity_ * mouse_delta.x_;
        rotation_y += mouse_sensitivity_ * mouse_delta.y_;
    }

    Quaternion rotation = transform.rotation;
    rotation = Quaternion{rotation_x * deltaSeconds * look_speed_, Vector{0.0f, 1.0f, 0.0f}} * rotation;
    rotation = rotation * Quaternion{rotation_y * deltaSeconds * look_speed_, Vector{1.0f, 0.0f, 0.0f}};
    SetRotation(rotation);
    Translate((rotation * movement) * deltaSeconds * move_speed_);

    ImGui::Begin("Camera");
    if (ImGui::Button("Reset Camera"))
    {
        transform = Transform();
        Translate(Vector{ 0.0f, 4.0f, 10.0f });
        Rotate(Vector{ 0.0f, 180.0f, 0.0f });
    }
    ImGui::End();
}
