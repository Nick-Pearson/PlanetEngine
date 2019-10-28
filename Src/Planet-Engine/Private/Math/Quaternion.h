#pragma once
#include "Vector.h"
#include "Math.h"
#include <DirectXMath.h>

__declspec(align(16))
struct Quaternion
{
public:

	DirectX::XMVECTOR value;

	Quaternion()
	{
		value = DirectX::XMQuaternionIdentity();
	}

	Quaternion(const Vector& eulerAngles)
	{
		value = DirectX::XMQuaternionRotationRollPitchYaw(Math::DegToRad(eulerAngles.x), Math::DegToRad(eulerAngles.y), Math::DegToRad(eulerAngles.z));
	}

public:

	void operator+=(const Vector& euler)
	{
		*this += Quaternion(euler);
	}

	void operator+=(const Quaternion& other)
	{
		value = DirectX::XMQuaternionMultiply(other.value, value);
	}

	Vector operator*(Vector other) const
	{
		return Vector(DirectX::XMVector3Rotate(other.ToVectorReg(), value));
	}

public:

	inline void Normalise()
	{
		value = DirectX::XMQuaternionNormalize(value);
	}

	inline bool IsNormalised() const
	{
		return SizeSqrd() == 1.0f;
	}

	inline float SizeSqrd() const
	{
		return DirectX::XMVectorGetX(DirectX::XMQuaternionLengthSq(value));
	}

	inline float Size() const
	{
		return Math::Sqrt(SizeSqrd());
	}

private:

};