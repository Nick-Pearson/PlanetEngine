#pragma once

#include <memory>

struct Colour
{
 public:
    Colour() :
        Colour(0, 0, 0, 255)
    {}

    Colour(uint8_t r, uint8_t g, uint8_t b, uint8_t a) :
        r(r), g(g), b(b), a(a)
    {}

    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

struct TextureData
{
    inline Colour GetPixel(int x, int y) const { return data_[x + (y * width_)]; }
    void SetPixel(int x, int y, Colour value);
    void Fill(Colour value);

    inline Colour* GetData() const { return data_.get(); }

    inline int GetWidth() const { return width_; }
    inline int GetHeight() const { return height_; }

 private:
    int width_;
    int height_;
    std::unique_ptr<Colour[]> data_;
};