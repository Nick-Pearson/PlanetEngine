#pragma once

#include <d3d12.h>

class TextureResource
{
 public:
    virtual bool IsLoaded() const = 0;
};