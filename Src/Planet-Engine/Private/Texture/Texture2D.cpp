#include "Texture/Texture2D.h"
#include "Texture/Texture.h"

Texture2D::Texture2D(int width, int height) :
    width_(width),
    height_(height),
    Texture(TextureDimensions::_2D, TextureDataType::STATIC)
{
    data_ = std::make_unique<Colour[]>(width * height);
}

void Texture2D::SetPixel(int x, int y, Colour value)
{
    data_[x + (y * width_)] = value;
}

void Texture2D::Fill(Colour value)
{
    for (int x = 0; x < width_; ++x)
    {
        for (int y = 0; y < height_; ++y)
        {
            SetPixel(x, y, value);
        }
    }
}
