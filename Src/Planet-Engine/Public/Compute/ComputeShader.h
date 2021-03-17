#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Texture/ComputeTexture2D.h"

struct NumThreads
{
    NumThreads(int x, int y, int z) :
        x_(x), y_(y), z_(z)
    {}

    int x_, y_, z_;
};

class ComputeShader
{
 public:
    explicit ComputeShader(const char* shader_name, const NumThreads& num_threads);

    inline std::string GetShaderName() const { return shader_name_; }
    inline NumThreads GetNumThreads() const { return num_threads_; }
    inline int GetNumTextureOutputs() const { return texture_outputs_.size(); }
    inline const Texture* GetTextureOutput(int slot) const { return texture_outputs_[slot].get(); }

    void AddTextureOutput(const std::shared_ptr<ComputeTexture2D>& texture);
 private:
    std::string shader_name_;
    NumThreads num_threads_;
    std::vector<std::shared_ptr<Texture>> texture_outputs_;
};