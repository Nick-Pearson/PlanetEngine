#pragma once

#include <valarray>
#include <DirectXMath.h>

struct Vector
{
public:
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	Vector(float inX = 0.0f, float inY = 0.0f, float inZ = 0.0f) : 
		x(inX), y(inY), z(inZ)
	{}
	
	Vector(const DirectX::XMVECTOR& vecReg)
	{
		x = DirectX::XMVectorGetX(vecReg);
		y = DirectX::XMVectorGetY(vecReg);
		z = DirectX::XMVectorGetZ(vecReg);
	}

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

	Vector operator*(const Vector& other)
	{
		Vector res = *this;
		res *= other;
		return res;
	}

	Vector& operator*=(const Vector& other)
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
		return *this;
	}

	Vector operator*(float other)
	{
		Vector res = *this;
		res *= other;
		return res;
	}

	Vector& operator*=(float value)
	{
		x *= value;
		y *= value;
		z *= value;
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
	
	Vector operator-()
	{
		return *this * -1.0f;
	}

	Vector operator-(float other)
	{
		Vector res = *this;
		res *= other;
		return res;
	}

	Vector& operator-=(float value)
	{
		x -= value;
		y -= value;
		z -= value;
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

	DirectX::XMVECTOR ToVectorReg() const
	{
		return DirectX::XMVectorSet(x, y, z, 1.0f);
	}

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

	bool IsZero() const
	{
		return x == 0.0f && y == 0.0f && z == 0.0f;
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