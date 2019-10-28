#pragma once

#include <memory>

struct Colour
{
public:
	Colour() :
		Colour(0, 0, 0, 255)
	{}

	Colour(unsigned char r,	unsigned char g, unsigned char b, unsigned char a) :
		r(r), g(g), b(b), a(a)
	{}

	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

class Texture2D
{
public:
	Texture2D(unsigned int width, unsigned int height);

	inline Colour GetPixel(unsigned int x, unsigned int y) const { return mData[x + (y * mWidth)]; }
	void SetPixel(unsigned int x, unsigned int y, Colour value);
	void Fill(Colour value);

	inline Colour* GetData() const { return mData.get(); }

	inline unsigned int GetWidth() const { return mWidth; }
	inline unsigned int GetHeight() const { return mHeight; }

private:

	unsigned int mWidth;
	unsigned int mHeight;
	std::unique_ptr<Colour[]> mData;
};