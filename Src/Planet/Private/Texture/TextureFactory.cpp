#include "TextureFactory.h"

#include "Texture2D.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::shared_ptr<Texture2D> TextureFactory::fromFile(const char* filepath)
{
	int width, height, components;
	unsigned char *data = stbi_load(filepath, &width, &height, &components, 4);

	std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>(width, height);
	memcpy(texture->GetData(), data, width * height * sizeof(unsigned char) * 4);

	stbi_image_free(data);
	return texture;
}
