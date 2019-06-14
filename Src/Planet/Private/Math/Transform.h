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
};