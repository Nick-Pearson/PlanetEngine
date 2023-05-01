#include "Compute/ComputeShader.h"
#include "PlanetLogging.h"

ComputeShader::ComputeShader(const char* shader_path, const NumThreads& num_threads) :
    shader_path_(shader_path), num_threads_(num_threads)
{
}

void ComputeShader::AddTextureOutput(const std::shared_ptr<ComputeTexture2D>& texture)
{
    texture_outputs_.push_back(texture);
}

void ComputeShader::AddTextureOutput(const std::shared_ptr<ComputeTexture3D>& texture)
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