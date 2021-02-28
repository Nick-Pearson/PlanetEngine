#include "Texture2D.h"

Texture2D::Texture2D(unsigned int width, unsigned int height) :
    mWidth(width), mHeight(height)
{
    mData = std::make_unique<Colour[]>(width * height);
}

void Texture2D::SetPixel(unsigned int x, unsigned int y, Colour value)
{
    mData[x + (y * mWidth)] = value;
}

void Texture2D::Fill(Colour value)
{
    for (unsigned int x = 0; x < mWidth; ++x)
    {
        for (unsigned int y = 0; y < mHeight; ++y)
        {
            SetPixel(x, y, value);
        }
    }
}
