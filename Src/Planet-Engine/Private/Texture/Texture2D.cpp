#include "Texture/Texture2D.h"
#include "Texture/Texture.h"

Texture2D::Texture2D(int width, int height) :
    mWidth(width),
    mHeight(height),
    Texture(TextureDimensions::_2D, TextureDataType::STATIC)
{
    mData = std::make_unique<Colour[]>(width * height);
}

void Texture2D::SetPixel(int x, int y, Colour value)
{
    mData[x + (y * mWidth)] = value;
}

void Texture2D::Fill(Colour value)
{
    for (int x = 0; x < mWidth; ++x)
    {
        for (int y = 0; y < mHeight; ++y)
        {
            SetPixel(x, y, value);
        }
    }
}
