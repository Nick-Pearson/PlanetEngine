#pragma once

#include <memory>
#include <vector>

#include "MaterialResource.h"
#include "../Texture/D3DTexture.h"
#include "../Shader/D3DPixelShader.h"

class D3DMaterial : public MaterialResource
{
 public:
    D3DMaterial(const D3DPixelShader* shader, const std::vector<std::shared_ptr<D3DTexture>>& textures, bool alpha);

    inline const D3DPixelShader* GetPixelShader() const override { return shader_; }
    inline bool IsLoaded() const override { return true; }

 private:
    const D3DPixelShader* const shader_;
    std::vector<std::shared_ptr<D3DTexture>> textures_;
    bool alpha_;
};