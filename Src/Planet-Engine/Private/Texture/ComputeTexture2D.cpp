#include "Texture/ComputeTexture2D.h"

ComputeTexture2D::ComputeTexture2D(unsigned int width, unsigned int height) :
    width_(width),
    height_(height),
    Texture(TextureDimensions::_2D, TextureDataType::COMPUTE)
{
}