#pragma once

#include <cstdint>
#include <string>

class Texture
{
 public:
    explicit Texture(unsigned int width, unsigned int height = 0, unsigned int depth = 0);

    unsigned int GetDimensions() const;
    inline unsigned int GetWidth() const { return width_; }
    inline unsigned int GetHeight() const { return height_; }
    inline unsigned int GetDepth() const { return depth_; }

    boolean gpu_write_ = false;
    boolean cpu_read_ = false;
 private:
    unsigned int width_ = 0;
    unsigned int height_ = 0;
    unsigned int depth_ = 0;
};