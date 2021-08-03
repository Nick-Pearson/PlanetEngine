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

    explicit Quaternion(const Vector& eulerAngles)
    {
        value = DirectX::XMQuaternionRotationRollPitchYaw(
            Math::DegToRad(eulerAngles.x),
            Math::DegToRad(eulerAngles.y),
            Math::DegToRad(eulerAngles.z));
    }

    Quaternion(const float angle, const Vector& axis)
    {
        value = DirectX::XMQuaternionRotationAxis(
            axis.ToVectorReg(),
            angle);
    }

 public:
    void operator*=(const Quaternion& other)
    {
        value = DirectX::XMQuaternionMultiply(other.value, value);
    }

    Quaternion operator*(Quaternion other) const
    {
        auto q = Quaternion();
        q.value = DirectX::XMQuaternionMultiply(other.value, value);
        return q;
    }

    Vector operator*(Vector other) const
    {
        return Vector(DirectX::XMVector3Rotate(other.ToVectorReg(), value));
    }

    bool operator==(const Quaternion& other) const
    {
        // TODO: Does this account for Quaternions having multiple representations for the same angle?
        return DirectX::XMVector3Equal(value, other.value);
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
};