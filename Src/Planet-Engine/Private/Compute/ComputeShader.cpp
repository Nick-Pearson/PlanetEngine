#include "Compute/ComputeShader.h"

ComputeShader::ComputeShader(const char* shader_name, const NumThreads& num_threads) :
    shader_name_(shader_name), num_threads_(num_threads)
{
}

void ComputeShader::AddTextureOutput(const std::shared_ptr<ComputeTexture2D>& texture)
{
    texture_outputs_.push_back(texture);
}

void ComputeShader::AddDataInput(const std::shared_ptr<DataBuffer>& data)
{
    data_outputs_.push_back(data);
}

void ComputeShader::AddDefine(const std::string& key, const std::string& val)
{
    defines_.emplace(key, val);
}