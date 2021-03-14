#pragma once

#include <string>
#include <memory>
#include <vector>

class Texture;

class Material
{
 public:
    explicit Material(const char* shaderName);

    inline std::string GetShaderPath() const { return mShaderPath;  }

    void AddTexture(std::shared_ptr<Texture> texture);
    inline const Texture* GetTextureAt(int slot) const { return mTextures[slot].get(); }
    inline int GetNumTextures() const { return mTextures.size(); }
    inline void EnableAlphaBlending() { mAlpha = true; }
    inline bool IsAlphaBlendingEnabled() const { return mAlpha; }

 private:
    bool mAlpha = false;
    std::string mShaderPath;
    std::vector<std::shared_ptr<Texture>> mTextures;
};