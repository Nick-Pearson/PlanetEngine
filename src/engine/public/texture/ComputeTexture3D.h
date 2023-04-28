#pragma once

#include <string>

#include "Texture.h"

class ComputeTexture3D : public Texture
{
 public:
    ComputeTexture3D(unsigned int width, unsigned int height, unsigned int depth);

    inline unsigned int GetWidth() const { return width_; }
    inline unsigned int GetHeight() const { return height_; }
    inline unsigned int GetDepth() const { return depth_; }

 private:
    unsigned int width_;
    unsigned int height_;
    unsigned int depth_;
};