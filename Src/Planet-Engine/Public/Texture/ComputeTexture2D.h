#pragma once

#include <string>

#include "Texture.h"

class ComputeTexture2D : public Texture
{
 public:
    ComputeTexture2D(unsigned int width, unsigned int height);

    inline unsigned int GetWidth() const { return width_; }
    inline unsigned int GetHeight() const { return height_; }

 private:
    unsigned int width_;
    unsigned int height_;
};