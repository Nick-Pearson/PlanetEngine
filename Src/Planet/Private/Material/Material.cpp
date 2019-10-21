#include "Material.h"

Material::Material(const char* shaderName) :
	mShaderPath(shaderName)
{
}

void Material::AddTexture(std::shared_ptr<Texture2D> texture)
{
	mTextures.push_back(texture);
}
