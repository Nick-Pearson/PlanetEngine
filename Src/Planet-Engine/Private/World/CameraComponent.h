#pragma once

#include "../Entity/Component.h"

class CameraComponent : public Component
{
 public:
    float NearClip = 0.5f;
    float FarClip = 5000.0f;
};