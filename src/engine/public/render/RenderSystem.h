#pragma once

#include "Renderer.h"
#include "World/CameraComponent.h"
#include "Compute/ComputeShader.h"
#include "Texture/Texture2D.h"

class RenderSystem
{
 public:
    // called when the render system is loaded
    virtual void Load(class PlanetEngine* engine) = 0;
    // called when the render system is unloaded
    virtual void UnLoad(class PlanetEngine* engine) = 0;

    // Applies changes to the scene to the renderer
    virtual void ApplyQueue(const struct RenderQueueItems& items) = 0;

    virtual void RenderFrame(const CameraComponent& camera) = 0;
    virtual void RenderToTexture(Texture2D* texture, const CameraComponent& camera) = 0;

    virtual void InvokeCompute(const ComputeShader* shader) = 0;

    virtual Renderer* GetRenderer() const = 0;
};