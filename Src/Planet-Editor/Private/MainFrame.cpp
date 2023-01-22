#include "MainFrame.h"

#include "AssetViewer/AssetViewer.h"
#include "GameCanvas.h"

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(wxID_EXIT,  MainFrame::OnExit)
    EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(FileAssetManager* asset_manager)
        : wxFrame(NULL, wxID_ANY, "Planet Editor", wxPoint(50, 50))
{
    wxMenu *menuFile = new wxMenu;
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
    SetMenuBar(menuBar);

    CreateStatusBar();
    SetStatusText("Welcome to wxWidgets!");

    int width, height;
    GetClientSize(&width, &height);

    wxBoxSizer* v_sizer = new wxBoxSizer{wxVERTICAL};

    wxToolBar* toolbar = new wxToolBar{this, wxID_ANY};
    toolbar->AddTool(wxID_ANY, wxT("Play"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, nullptr);
    toolbar->AddTool(wxID_ANY, wxT("Pause"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, nullptr);
    v_sizer->Add(toolbar, 0, wxEXPAND, 5);

    wxBoxSizer* h_sizer = new wxBoxSizer{wxHORIZONTAL};

    GameCanvas* canvas = new GameCanvas{this};
    h_sizer->Add(canvas, 1, wxALL, 5);

    wxNotebook* tree_page = new wxNotebook{this, wxID_ANY};
    wxPanel* scene = new wxPanel(tree_page, wxID_ANY);
    scene->SetMinSize(wxSize(200, 200));
    AssetViewer* assets = new AssetViewer{tree_page, asset_manager};
    asset_manager->AddListener(assets);

    tree_page->AddPage(scene, wxT("Scene"));
    tree_page->AddPage(assets, wxT("Assets"));
    h_sizer->Add(tree_page, 0, wxEXPAND);

    wxNotebook* props_page = new wxNotebook{this, wxID_ANY};
    wxPanel* props = new wxPanel(props_page, wxID_ANY);
    props_page->AddPage(props, wxT("Properties"));
    h_sizer->Add(props_page, 0, wxEXPAND);

    v_sizer->Add(h_sizer, 0, wxEXPAND);

    SetSizerAndFit(v_sizer);
    SetAutoLayout(true);
}

void MainFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a wxWidgets' Hello world sample",
                  "About Hello World", wxOK | wxICON_INFORMATION);
}