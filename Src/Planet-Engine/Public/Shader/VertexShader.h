#pragma once

#include <string>
#include <vector>

#include "ShaderIO.h"

class VertexShader
{
 public:
    explicit VertexShader(const char* shader_path);

    inline std::string GetShaderPath() const { return shader_path_;  }

 private:
    std::string shader_path_;
};
