#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/treectrl.h>

#include "../File/FileAssetManager.h"

class AssetViewer : public wxPanel, public DirectoryListener
{
 public:
    AssetViewer(wxWindow *parent, const FileAssetManager* asset_manager);

    void OnFileAdded() final;
    void OnFileUpdated() final;
    void OnFileRemoved() final;

 private:
    void Draw();
    void DrawChildrenRecursive(wxTreeItemId parent, const Directory& directory);

    const FileAssetManager* const asset_manager_;
    wxTreeCtrl* tree_;
};
