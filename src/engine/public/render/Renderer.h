#pragma once

#include "Render/WorldBufferData.h"
class Renderer
{
 public:
    virtual void UpdateWorldBuffer(const WorldBufferData& worldBufferData) = 0;
};