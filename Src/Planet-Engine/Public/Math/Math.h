#pragma once

#include <cmath>

#define PI 3.14159265359f

class Math
{
 public:
    static inline float DegToRad(float deg)
    {
        return deg * PI / 180.0f;
    }

    static inline float RadToDeg(float rad)
    {
        return rad * 180.0f / PI;
    }

    static inline float Sin(float angleRad)
    {
        return std::sin(angleRad);
    }

    static inline float Cos(float angleRad)
    {
        return std::cos(angleRad);
    }

    static inline float ASin(float value)
    {
        return std::asin(value);
    }

    static inline float ACos(float value)
    {
        return std::acos(value);
    }

    static inline void SinAndCos(float angleRad, float* outSin, float* outCos)
    {
        *outSin = std::sin(angleRad);
        *outCos = std::cos(angleRad);
    }

    static inline float Sqrt(float value)
    {
        return std::sqrt(value);
    }
};