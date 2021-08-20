#pragma once

#include <memory>

#include "Texture.h"

struct Colour
{
 public:
    Colour() :
        Colour(0, 0, 0, 255)
    {}

    Colour(unsigned char r, unsigned char g, unsigned char b, unsigned char a) :
        r(r), g(g), b(b), a(a)
    {}

    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

class Texture2D : public Texture
{
 public:
    Texture2D(int width, int height);

    inline Colour GetPixel(int x, int y) const { return mData[x + (y * mWidth)]; }
    void SetPixel(int x, int y, Colour value);
    void Fill(Colour value);

    inline Colour* GetData() const { return mData.get(); }

    inline int GetWidth() const { return mWidth; }
    inline int GetHeight() const { return mHeight; }

 private:
    int mWidth;
    int mHeight;
    std::unique_ptr<Colour[]> mData;
};