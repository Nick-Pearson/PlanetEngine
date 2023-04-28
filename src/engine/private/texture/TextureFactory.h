#pragma once

#include <memory>

class Texture2D;

class TextureFactory
{
 public:
    static std::shared_ptr<Texture2D> fromFile(const char* filepath);
};