#pragma once

#include <string>
#include <vector>
#include <functional>

#include "Asset/AssetLoader.h"

struct Node
{
    int id_;
    std::string path_;
    std::string name_;
    Asset* asset_;
};

struct Directory : public Node
{
    std::vector<Node> files_;
    std::vector<Directory> sub_directories_;

    Node* GetFile(const std::string& path);
    Directory* GetDirectory(const std::string& path);
};

class FileAssetManager
{
 public:
    FileAssetManager(AssetLoader* asset_loader, const std::string& project_path);
    ~FileAssetManager();

    inline const Directory* GetAssets() const { return &assets_root_; }

 private:
    void LoadChangedAssets();
    void LoadChangedAssetsRecursive(Directory* directory);

    void OnFileAdded(Node* file);
    void OnFileUpdated(Node* file);
    void OnFileRemoved(Node* file);

    Directory assets_root_;

    AssetLoader* asset_loader_;
    std::string assets_dir_;
    std::string data_dir_;
    std::string cache_dir_;
    int last_id_ = 0;
};