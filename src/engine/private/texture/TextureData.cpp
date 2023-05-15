#include "Texture/TextureData.h"

TextureData::TextureData(int width, int height) :
    width_(width),
    height_(height)
{
    data_ = std::make_unique<Colour[]>(width * height);
}

void TextureData::SetPixel(int x, int y, Colour value)
{
    data_[x + (y * width_)] = value;
}

void TextureData::Fill(Colour value)
{
    for (int x = 0; x < width_; ++x)
    {
        for (int y = 0; y < height_; ++y)
        {
            SetPixel(x, y, value);
        }
    }
}
