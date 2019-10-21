#pragma once

#include <valarray>
#include <DirectXMath.h>

struct Vector2D
{
public:
	float x = 0.0f;
	float y = 0.0f;

	Vector2D(float inX = 0.0f, float inY = 0.0f) :
		x(inX), y(inY)
	{}
	
	Vector2D(const DirectX::XMVECTOR& vecReg)
	{
		x = DirectX::XMVectorGetX(vecReg);
		y = DirectX::XMVectorGetY(vecReg);
	}

	Vector2D operator/(const Vector2D& other)
	{
		Vector2D res = *this;
		res /= other;
		return res;
	}

	Vector2D& operator/=(const Vector2D& other)
	{
		x /= other.x;
		y /= other.y;
		return *this;
	}

	Vector2D operator/(float other)
	{
		Vector2D res = *this;
		res /= other;
		return res;
	}

	Vector2D& operator/=(float value)
	{
		x /= value;
		y /= value;
		return *this;
	}

	Vector2D operator*(const Vector2D& other)
	{
		Vector2D res = *this;
		res *= other;
		return res;
	}

	Vector2D& operator*=(const Vector2D& other)
	{
		x *= other.x;
		y *= other.y;
		return *this;
	}

	Vector2D operator*(float other)
	{
		Vector2D res = *this;
		res *= other;
		return res;
	}

	Vector2D& operator*=(float value)
	{
		x *= value;
		y *= value;
		return *this;
	}

	Vector2D operator+(const Vector2D& other)
	{
		Vector2D res = *this;
		res += other;
		return res;
	}

	Vector2D& operator+=(const Vector2D& other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}
	
	Vector2D operator-()
	{
		return *this * -1.0f;
	}

	Vector2D operator-(float other)
	{
		Vector2D res = *this;
		res *= other;
		return res;
	}

	Vector2D& operator-=(float value)
	{
		x -= value;
		y -= value;
		return *this;
	}

	Vector2D operator-(const Vector2D& other)
	{
		Vector2D res = *this;
		res -= other;
		return res;
	}

	Vector2D& operator-=(const Vector2D& other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

public:

	DirectX::XMVECTOR ToVectorReg() const
	{
		return DirectX::XMVectorSet(x, y, 0.0f, 1.0f);
	}

	inline float Length() const
	{
		return std::sqrt(LengthSqrd());
	}

	float LengthSqrd() const
	{
		return (x * x) + (y * y);
	}

	void Normalise()
	{
		float len = Length();
		if (len < 0.001f) return;

		*this = *this / len;
	}

	bool IsZero() const
	{
		return x == 0.0f && y == 0.0f;
	}
};