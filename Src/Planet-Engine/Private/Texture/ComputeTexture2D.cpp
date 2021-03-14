#include "Texture/ComputeTexture2D.h"

ComputeTexture2D::ComputeTexture2D(unsigned int width, unsigned int height, const char* shader_name) :
    width_(width),
    height_(height),
    shader_name_(shader_name),
    Texture(TextureDimensions::_2D, TextureDataType::COMPUTE)
{
}