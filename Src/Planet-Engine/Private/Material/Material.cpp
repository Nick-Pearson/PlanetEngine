#include "Material/Material.h"

#include <memory>

Material::Material(const char* shader_name) :
    shader_path_(shader_name)
{
}

void Material::AddTexture(std::shared_ptr<Texture> texture)
{
    textures_.push_back(texture);
}
