#pragma once

#include "../EditorWindow.h"
#include "../FileAssetManager.h"

class AssetViewer : public EditorWindow
{
 public:
    explicit AssetViewer(const FileAssetManager* asset_manager);
    virtual ~AssetViewer();

    void Draw() final;

 private:
    void DrawTreeRecursive(const Directory& directory);

    const char* GetIconForAsset(const Node& file) const;

    const FileAssetManager* asset_manager_;
    int selected_file_ = 0;
};
