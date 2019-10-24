#include "TextureFactory.h"

#include "Texture2D.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::shared_ptr<Texture2D> TextureFactory::fromFile(const char* filepath)
{
	int x, y, n;

	unsigned char *data = stbi_load(filepath, &x, &y, &n, 4);
	// ... process data if not NULL ...
	// ... x = width, y = height, n = # 8-bit components per pixel ...
	// ... replace '0' with '1'..'4' to force that many components per pixel
	// ... but 'n' will always be the number that it would have been if you said 0

	std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>(x, y);
	memcpy(texture->GetData(), data, x * y * sizeof(unsigned char) * 4);

	stbi_image_free(data);
	return texture;
}
