#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "NumThreads.h"
#include "Texture/ComputeTexture2D.h"
#include "Texture/ComputeTexture3D.h"
#include "PlanetLogging.h"

struct DataBuffer
{
    DataBuffer(void* data, size_t struct_size, size_t count) :
        data_(data), count_(count), stride_(struct_size)
    {
        P_ASSERT(struct_size % 16 == 0, "data buffers must be 16 byte aligned");
    }

    void* data_;
    size_t count_;
    size_t stride_;
};

class ComputeShader
{
 public:
    explicit ComputeShader(const char* shader_name, const NumThreads& num_threads);

    inline std::string GetShaderName() const { return shader_name_; }
    inline NumThreads GetNumThreads() const { return num_threads_; }

    inline int GetNumTextureOutputs() const { return static_cast<int>(texture_outputs_.size()); }
    inline const Texture* GetTextureOutput(int slot) const { return texture_outputs_[slot].get(); }

    void AddTextureOutput(const std::shared_ptr<ComputeTexture2D>& texture);
    void AddTextureOutput(const std::shared_ptr<ComputeTexture3D>& texture);

    inline int GetNumDataInputs() const { return static_cast<int>(data_outputs_.size()); }
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