#pragma once

#include "ITexture.h"

class Texture2D : public ITexture
{
public:
	Texture2D(int inWidth, int inHeight);
	~Texture2D();

	inline int GetWidth() const { return width; }
	inline int GetHeight() const { return height; }

	// TODO: Iterator

private:

	int width;
	int height;
	Colour* data;
};