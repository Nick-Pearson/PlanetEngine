#pragma once

#include <ostream>

#include "Math/Vector.h"
#include "Math/Vector2D.h"

std::ostream& operator<<(std::ostream& os, const Vector& vec)
{
    return os << "[ " << vec.x << ", " << vec.y << ", " << vec.z << " ]";
}

std::ostream& operator<<(std::ostream& os, const Vector2D& vec)
{
    return os << "[ " << vec.x << ", " << vec.y << " ]";
}