#include "Material/Material.h"

#include <memory>

Material::Material(const char* shaderName) :
    mShaderPath(shaderName)
{
}

void Material::AddTexture(std::shared_ptr<Texture> texture)
{
    mTextures.push_back(texture);
}
