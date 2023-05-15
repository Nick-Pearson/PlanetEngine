#include "Texture/Texture.h"

Texture::Texture(unsigned int width, unsigned int height, unsigned int depth) :
    width_(width), height_(height), depth_(depth)
{
}

unsigned int Texture::GetDimensions() const
{
    if (depth_ > 0) return 3;
    if (height_ > 0) return 2;
    return 1;
}