#pragma once

class Texture2D;

class TextureWriter
{
 public:
    static void writeToFile(const char* filepath, const Texture2D& texture);
};