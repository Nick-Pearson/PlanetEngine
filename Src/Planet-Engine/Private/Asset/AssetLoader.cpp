#include "Asset/AssetLoader.h"

std::string ExtractFileExtension(const std::string& path)
{
    auto idx = path.find_last_of('.');
    if (idx == std::string::npos)
    {
        return nullptr;
    }
    return path.substr(idx + 1);
}

Asset* AssetLoader::LoadAsset(const std::string& file_path)
{
    const std::string file_extension = ExtractFileExtension(file_path);
    if (file_extension == "fbx")
    {
        return new Asset{AssetType::MESH, nullptr};
    }
    else if (file_extension == "obj")
    {
        return new Asset{AssetType::MESH, nullptr};
    }
    else if (file_extension == "jpg" ||
        file_extension == "png")
    {
        return new Asset{AssetType::TEXTURE, nullptr};
    }
    return nullptr;
}