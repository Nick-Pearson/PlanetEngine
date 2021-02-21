#pragma once

#include "../Mesh/GPUResourceManager.h"

#include <string>

class Texture2D;

class Material
{
public:
	Material(const char* shaderName);

	inline std::string GetShaderPath() const { return mShaderPath;  }

	void AddTexture(std::shared_ptr<Texture2D> texture);
	inline std::shared_ptr<Texture2D> GetTextureAt(int slot) const { return mTextures[slot]; }
	inline int GetNumTextures() const { return mTextures.size(); }

private:
	std::string mShaderPath;
	std::vector<std::shared_ptr<Texture2D>> mTextures;
};