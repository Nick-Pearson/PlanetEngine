#include "FileAssetManager.h"

#include <vector>
#include <algorithm>

#include "Platform/Platform.h"
#include "PlanetLogging.h"

Node* Directory::GetFile(const std::string& path)
{
    auto it = std::find_if(files_.begin(), files_.end(), [&path](const Node& f) {
        return f.path_ == path;
    });

    if (it == files_.end())
    {
        return nullptr;
    }
    return &(*it);
}

Directory* Directory::GetDirectory(const std::string& path)
{
    auto it = std::find_if(sub_directories_.begin(), sub_directories_.end(), [&path](const Directory& d) {
        return d.path_ == path;
    });

    if (it == sub_directories_.end())
    {
        return nullptr;
    }
    return &(*it);
}

FileAssetManager::FileAssetManager(AssetLoader* asset_loader, const std::string& project_path) :
    asset_loader_(asset_loader)
{
    assets_dir_ = project_path + "\\Assets";
    data_dir_ = project_path + "\\Data";
    cache_dir_ = project_path + "\\Cache";

    Platform::CreateDirectoryIfNotExists(assets_dir_.c_str());
    Platform::CreateDirectoryIfNotExists(data_dir_.c_str());
    Platform::CreateDirectoryIfNotExists(cache_dir_.c_str());

    assets_root_.path_ = assets_dir_;
    Platform::AddDirectoryChangeListener(assets_dir_.c_str(), [this]() { LoadChangedAssets(); });
    LoadChangedAssets();
}

FileAssetManager::~FileAssetManager()
{
}

void FileAssetManager::LoadChangedAssets()
{
    LoadChangedAssetsRecursive(&assets_root_);
}

void FileAssetManager::LoadChangedAssetsRecursive(Directory* directory)
{
    const auto parent_path_len = directory->path_.size() + 1;
    std::vector<std::string> files = Platform::ListFiles(directory->path_.c_str());
    for (auto node : directory->files_)
    {
        auto it = std::find(files.begin(), files.end(), node.path_);
        if (it == files.end())
        {
            OnFileRemoved(&node);
        }
    }

    for (auto path : files)
    {
        Node* file = directory->GetFile(path);
        if (file == nullptr)
        {
            Node new_file;
            new_file.path_ = path;
            new_file.name_ = path.substr(parent_path_len);
            new_file.id_ = last_id_++;
            directory->files_.push_back(new_file);

            file = &directory->files_[directory->files_.size() - 1];
            OnFileAdded(file);
        }
        else
        {
            OnFileUpdated(file);
        }
    }

    std::vector<std::string> directories = Platform::ListDirectories(directory->path_.c_str());
    for (auto path : directories)
    {
        Directory* dir = directory->GetDirectory(path);
        if (dir == nullptr)
        {
            P_LOG("Found new directory {}", path);
            Directory new_dir;
            new_dir.path_ = path;
            new_dir.name_ = path.substr(parent_path_len);
            new_dir.id_ = last_id_++;
            directory->sub_directories_.push_back(new_dir);

            dir = &directory->sub_directories_[directory->sub_directories_.size() - 1];
        }
        LoadChangedAssetsRecursive(dir);
    }
}

void FileAssetManager::OnFileAdded(Node* file)
{
    P_LOG("Found new file {}", file->path_);
    file->asset_ = asset_loader_->LoadAsset(file->path_);
}

void FileAssetManager::OnFileUpdated(Node* file)
{
    P_LOG("Found updated file {}", file->path_);
}

void FileAssetManager::OnFileRemoved(Node* file)
{
    P_LOG("Found removed file {}", file->path_);
}
