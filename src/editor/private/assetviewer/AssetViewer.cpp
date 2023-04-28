#include "AssetViewer.h"

AssetViewer::AssetViewer(wxWindow *parent, const FileAssetManager* asset_manager) :
    wxPanel(parent, wxID_ANY), asset_manager_(asset_manager)
{
    SetMinSize(wxSize(200, 200));

    tree_ = new wxTreeCtrl{this};

    wxBoxSizer* panel_sizer = new wxBoxSizer{wxHORIZONTAL};
    panel_sizer->Add(tree_, 1, wxEXPAND | wxALL);
    SetSizerAndFit(panel_sizer);

    Draw();
}

void AssetViewer::OnFileAdded()
{
    Draw();
}

void AssetViewer::OnFileUpdated()
{
    Draw();
}

void AssetViewer::OnFileRemoved()
{
    Draw();
}

void AssetViewer::Draw()
{
    tree_->DeleteAllItems();
    auto item_id = tree_->AddRoot("Assets");
    DrawChildrenRecursive(item_id, *asset_manager_->GetAssets());
}

void AssetViewer::DrawChildrenRecursive(wxTreeItemId parent, const Directory& directory)
{
    for (auto sub_dir : directory.sub_directories_)
    {
        auto item_id = tree_->AppendItem(parent, sub_dir.name_.c_str());
        DrawChildrenRecursive(item_id, sub_dir);
    }

    for (auto file : directory.files_)
    {
        tree_->AppendItem(parent, file.name_.c_str());
    }
}
