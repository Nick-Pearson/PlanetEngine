#pragma once

#include <memory>

#include "Entity/Entity.h"

class CameraComponent;

class FlyCam : public Entity
{
 public:
    FlyCam();

    inline const CameraComponent& GetCamera() const { return *camera; }

    void OnUpdate(float deltaSeconds) override;

 private:
    CameraComponent* camera;

    float move_speed_ = 10.0f;
    float look_speed_ = 0.5f;
    float mouse_sensitivity_ = 1.0f;
};