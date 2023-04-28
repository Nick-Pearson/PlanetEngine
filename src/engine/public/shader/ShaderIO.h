#pragma once

#include <string>

enum class ShaderParameterType : uint8_t
{
    TEXTURE_2D,
    TEXTURE_3D
};

struct ShaderInput
{
    ShaderParameterType type_;
    uint16_t slot_;
};
