#include "Material/Material.h"

#include <memory>

Material::Material(const PixelShader* pixel_shader) :
    pixel_shader_(pixel_shader)
{
}

void Material::AddTexture(std::shared_ptr<Texture> texture)
{
    textures_.push_back(texture);
}
