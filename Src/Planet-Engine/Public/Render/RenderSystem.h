#pragma once

#include "Renderer.h"
#include "World/CameraComponent.h"
#include "Compute/ComputeShader.h"

class RenderSystem
{
 public:
    virtual void Load(class PlanetEngine* engine) = 0;
    virtual void UnLoad(class PlanetEngine* engine) = 0;

    virtual void ApplyQueue(const struct RenderQueueItems& items) = 0;

    virtual void RenderFrame(const CameraComponent& camera) = 0;

    virtual void InvokeCompute(const ComputeShader& shader) = 0;

    virtual Renderer* GetRenderer() const = 0;
};