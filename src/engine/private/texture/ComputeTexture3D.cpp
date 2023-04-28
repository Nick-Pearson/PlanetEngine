#include "Texture/ComputeTexture3D.h"

ComputeTexture3D::ComputeTexture3D(unsigned int width, unsigned int height, unsigned int depth) :
    width_(width),
    height_(height),
    depth_(depth),
    Texture(TextureDimensions::_3D, TextureDataType::COMPUTE)
{
}