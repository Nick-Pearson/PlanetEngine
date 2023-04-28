#pragma once

#include <cstdint>
#include <string>

enum class TextureDataType : uint8_t
{
    STATIC,
    DYNAMIC,
    COMPUTE
};

enum class TextureDimensions : uint8_t
{
    _1D,
    _2D,
    _3D
};

class Texture
{
 public:
    Texture(TextureDimensions dimensions, TextureDataType data_type);

    inline TextureDimensions GetDimensions() const { return dimensions_; }
    inline TextureDataType GetDataType() const { return data_type_; }
 private:
    TextureDimensions dimensions_;
    TextureDataType data_type_;
};