#pragma once

#include "Render/WorldBufferData.h"
#include "Render/RenderState.h"

class Renderer
{
 public:
    virtual RenderState* AddRenderState(const RenderState& state) = 0;
    virtual void RemoveRenderState(const RenderState* state) = 0;

    virtual void UpdateWorldBuffer(const WorldBufferData& worldBufferData) = 0;
};