#pragma once

#include "Vector.h"
#include <DirectXMath.h>
#include "Quaternion.h"

struct Transform
{
public:

	Transform();

	DirectX::XMMATRIX GetMatrix() const;

	Quaternion rotation;
	Vector location;
	Vector scale;

	bool operator==(const Transform& other) const
	{
		return rotation == other.rotation &&
			location == other.location &&
			scale == other.scale;
	}
};