#include "Texture2D.h"

Texture2D::Texture2D(int inWidth, int inHeight) :
	width(inWidth), height(inHeight)
{
	if (width < 0) width = 0;
	if (height < 0) height = 0;

	data = new Colour[width * height];
}

Texture2D::~Texture2D()
{
	delete[] data;
}
