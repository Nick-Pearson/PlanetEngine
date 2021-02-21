#pragma once

#define PI 3.141592f

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

	static inline void SinAndCos(float angleRad, float& outSin, float& outCos)
	{
		outSin = Math::Sin(angleRad);
		outCos = Math::Cos(angleRad);
	}

	static inline float Sqrt(float value)
	{
		return std::sqrt(value);
	}
};