#pragma once

class Elipsoid
{
public:
	Elipsoid(float SizeX, float SizeY, float SizeZ);
	inline Elipsoid(float size = 1.0f) : Elipsoid(size, size, size) {}

	float sizeX, sizeY, sizeZ;
};