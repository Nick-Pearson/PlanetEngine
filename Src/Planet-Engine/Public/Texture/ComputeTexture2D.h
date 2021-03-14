#pragma once

#include <string>

#include "Texture.h"

class ComputeTexture2D : public Texture
{
 public:
    ComputeTexture2D(unsigned int width, unsigned int height, const char* shader_name);

    inline unsigned int GetWidth() const { return width_; }
    inline unsigned int GetHeight() const { return height_; }
    inline std::string GetShaderName() const { return shader_name_;  }

 private:
    unsigned int width_;
    unsigned int height_;
    std::string shader_name_;
};