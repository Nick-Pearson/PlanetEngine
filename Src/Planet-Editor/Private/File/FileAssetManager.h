#pragma once

#include <string>
#include <vector>
#include <functional>

#include "DirectoryListener.h"

struct Node
{
    int id_;
    std::string path_;
    std::string name_;
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
    explicit FileAssetManager(const std::string& project_path);
    ~FileAssetManager();

    inline const Directory* GetAssets() const { return &assets_root_; }

    void AddListener(DirectoryListener* listener);
    void RemoveListener(DirectoryListener* listener);

 private:
    void LoadChangedAssets();
    void LoadChangedAssetsRecursive(Directory* directory);

    void OnFileAdded(Node* file);
    void OnFileUpdated(Node* file);
    void OnFileRemoved(Node* file);

    Directory assets_root_;

    std::vector<DirectoryListener*> listeners_;
    std::string assets_dir_;
    std::string data_dir_;
    std::string cache_dir_;
    int last_id_ = 0;
};