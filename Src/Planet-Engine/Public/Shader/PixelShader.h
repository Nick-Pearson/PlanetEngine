#pragma once

#include <string>
#include <vector>

#include "ShaderIO.h"

class PixelShader
{
 public:
    explicit PixelShader(const char* shader_path);

    inline std::string GetShaderPath() const { return shader_path_;  }

    inline void EnableAlphaBlending() { alpha_blending_ = true; }
    inline bool IsAlphaBlendingEnabled() const { return alpha_blending_; }

    void AddInput(ShaderParameterType type);
    inline const ShaderInput* GetInputAt(int slot) const { return &inputs_[slot]; }
    inline int GetNumInputs() const { return inputs_.size(); }

 private:
    bool alpha_blending_ = false;
    std::string shader_path_;

    std::vector<ShaderInput> inputs_;
};
