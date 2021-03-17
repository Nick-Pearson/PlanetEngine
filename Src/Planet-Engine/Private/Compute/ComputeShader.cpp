#include "Compute/ComputeShader.h"

ComputeShader::ComputeShader(const char* shader_name, const NumThreads& num_threads) :
    shader_name_(shader_name), num_threads_(num_threads)
{
}

void ComputeShader::AddTextureOutput(const std::shared_ptr<ComputeTexture2D>& texture)
{
    texture_outputs_.push_back(texture);
}