#pragma once

#include "../Shader/D3DPixelShader.h"

class MaterialResource
{
 public:
    virtual const D3DPixelShader* GetPixelShader() const = 0;
    virtual bool IsLoaded() const = 0;
};