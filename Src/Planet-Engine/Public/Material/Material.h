#pragma once

#include <memory>
#include <vector>

class Texture;
class PixelShader;

class Material
{
 public:
    explicit Material(const PixelShader* pixel_shader);

    void AddTexture(std::shared_ptr<Texture> texture);
    inline const Texture* GetTextureAt(int slot) const { return textures_[slot].get(); }
    inline int GetNumTextures() const { return textures_.size(); }

    inline const PixelShader* GetPixelShader() const { return pixel_shader_; }

 private:
    const PixelShader* pixel_shader_;
    std::vector<std::shared_ptr<Texture>> textures_;
};
