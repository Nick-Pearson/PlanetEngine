#pragma once

#include "Vector.h"
#include <DirectXMath.h>

struct Transform
{
public:

	Transform();

	DirectX::XMMATRIX GetMatrix() const;

	Vector location;
	//Quaternion rotation;
	Vector scale;
};