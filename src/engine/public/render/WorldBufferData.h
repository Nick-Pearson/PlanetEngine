#pragma once

#include "Math/Vector.h"

struct WorldBufferData
{
    WorldBufferData() {}
    WorldBufferData(const Vector& inDir, float inStrength, const Vector& inCol) :
        sunDir(inDir), sunSkyStrength(inStrength), sunCol(inCol)
    {}

    Vector sunDir = Vector{ 0.0f, 1.0f, 0.0f };
    float sunSkyStrength = 20.0f;

    Vector sunCol = Vector{ 1.0f, 1.0f, 1.0f };
    char padding[4];
};
