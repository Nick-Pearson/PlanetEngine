#pragma once

#include <valarray>

struct Vector
{
public:
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	Vector operator/(const Vector& other)
	{
		Vector res = *this;
		res /= other;
		return res;
	}

	Vector& operator/=(const Vector& other)
	{
		x /= other.x;
		y /= other.y;
		z /= other.z;
		return *this;
	}

	Vector operator/(float other)
	{
		Vector res = *this;
		res /= other;
		return res;
	}

	Vector& operator/=(float value)
	{
		x /= value;
		y /= value;
		z /= value;
		return *this;
	}

	Vector operator+(const Vector& other)
	{
		Vector res = *this;
		res += other;
		return res;
	}

	Vector& operator+=(const Vector& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	Vector operator-(const Vector& other)
	{
		Vector res = *this;
		res -= other;
		return res;
	}

	Vector& operator-=(const Vector& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

public:

	inline float Length() const
	{
		return std::sqrt(LengthSqrd());
	}

	float LengthSqrd() const
	{
		return (x * x) + (y * y) + (z * z);
	}

	void Normalise()
	{
		float len = Length();
		if (len < 0.001f) return;

		*this = *this / len;
	}

	Vector Cross(const Vector& other) const
	{
		Vector result;
		result.x = (y * other.z) - (z * other.y);
		result.y = (z * other.x) - (x * other.z);
		result.z = (x * other.y) - (y * other.x);

		return result;
	}
};