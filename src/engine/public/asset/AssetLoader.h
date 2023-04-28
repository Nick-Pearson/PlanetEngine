#pragma once

#include <string>

enum class AssetType : uint8_t
{
    MESH,
    TEXTURE
};

struct Asset
{
    const AssetType type_;
    const void* data_;

    inline bool IsLoaded() const { return data_ != nullptr; }
};

class AssetLoader
{
 public:
    Asset* LoadAsset(const std::string& file_path);
};
