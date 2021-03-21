#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "Texture/ComputeTexture2D.h"

struct DataBuffer
{
    DataBuffer(void* data, size_t struct_size, size_t count) :
        data_(data), length_(struct_size * count), stride_(struct_size)
    {}

    void* data_;
    size_t length_;
    size_t stride_;
};

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

    inline int GetNumDataInputs() const { return data_outputs_.size(); }
    inline const DataBuffer* GetDataInput(int slot) const { return data_outputs_[slot].get(); }
    void AddDataInput(const std::shared_ptr<DataBuffer>& data);

    void AddDefine(const std::string& key, const std::string& val);
    inline const std::unordered_map<std::string, std::string> GetDefines() const { return defines_; }
 private:
    std::string shader_name_;
    NumThreads num_threads_;
    std::vector<std::shared_ptr<Texture>> texture_outputs_;
    std::vector<std::shared_ptr<DataBuffer>> data_outputs_;
    std::unordered_map<std::string, std::string> defines_;
};