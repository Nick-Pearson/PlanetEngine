#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/notebook.h>

class MainFrame: public wxFrame
{
 public:
    explicit MainFrame(class FileAssetManager* asset_manager);
 private:

    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    wxDECLARE_EVENT_TABLE();
};