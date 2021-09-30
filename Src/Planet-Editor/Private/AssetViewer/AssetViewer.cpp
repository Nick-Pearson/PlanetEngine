#include "AssetViewer.h"

#include "imgui.h"
#include "../IconsFontAwesome4.h"

AssetViewer::AssetViewer(const FileAssetManager* asset_manager) :
    asset_manager_(asset_manager)
{
}

AssetViewer::~AssetViewer()
{
}

void AssetViewer::Draw()
{
    // ImGui::Push
    // ImGui::Text("/");

    ImGui::BeginChild("Scrolling");
    DrawTreeRecursive(*asset_manager_->GetAssets());
    ImGui::EndChild();
}


void AssetViewer::DrawTreeRecursive(const Directory& directory)
{
    for (auto sub_dir : directory.sub_directories_)
    {
        if (ImGui::TreeNodeEx(sub_dir.name_.c_str()))
        {
            DrawTreeRecursive(sub_dir);
            ImGui::TreePop();
        }
    }

    char name_buff[256];
    for (auto file : directory.files_)
    {
        std::snprintf(name_buff, sizeof(name_buff), "  %s %s", GetIconForAsset(file), file.name_.c_str());

        bool selected = ImGui::Selectable(name_buff, selected_file_ == file.id_);
        if (selected)
        {
            selected_file_ = file.id_;
        }
    }
}

const char* AssetViewer::GetIconForAsset(const Node& file) const
{
    if (file.asset_ == nullptr)
    {
        return ICON_FA_QUESTION;
    }

    switch (file.asset_->type_)
    {
        case AssetType::MESH:
            return ICON_FA_CUBE;
        case AssetType::TEXTURE:
            return ICON_FA_PICTURE_O;
        default:
            return ICON_FA_QUESTION_CIRCLE;
    }
}