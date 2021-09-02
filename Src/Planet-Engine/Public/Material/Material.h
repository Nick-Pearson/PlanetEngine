#pragma once

#include <string>
#include <memory>
#include <vector>

class Texture;

class Material
{
 public:
    explicit Material(const char* shader_name);

    inline std::string GetShaderPath() const { return shader_path_;  }

    void AddTexture(std::shared_ptr<Texture> texture);
    inline const Texture* GetTextureAt(int slot) const { return textures_[slot].get(); }
    inline int GetNumTextures() const { return textures_.size(); }
    inline void EnableAlphaBlending() { alpha_blending_ = true; }
    inline bool IsAlphaBlendingEnabled() const { return alpha_blending_; }

 private:
    bool alpha_blending_ = false;
    std::string shader_path_;
    std::vector<std::shared_ptr<Texture>> textures_;
};