#include "Shader/PixelShader.h"

PixelShader::PixelShader(const char* shader_path) :
    shader_path_(shader_path)
{
}

void PixelShader::AddInput(ShaderParameterType type)
{
    ShaderInput input;
    input.type_ = type;
    input.slot_ = inputs_.size();

    inputs_.push_back(input);
}
