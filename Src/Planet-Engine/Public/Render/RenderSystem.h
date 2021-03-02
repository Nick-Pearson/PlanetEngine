#pragma once

#include "Renderer.h"
#include "ResourceManager.h"
#include "World/CameraComponent.h"

class RenderSystem
{
 public:
    virtual void Load(class PlanetEngine* engine) = 0;
    virtual void UnLoad(class PlanetEngine* engine) = 0;

    virtual void RenderFrame(const CameraComponent& camera) = 0;

    virtual Renderer* GetRenderer() = 0;

    virtual ResourceManager* GetResourceManager() = 0;
};