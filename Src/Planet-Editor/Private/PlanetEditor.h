#pragma once

#include <string>

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "PlanetEngine.h"

class PlanetEditor: public wxApp
{
 public:
    virtual bool OnInit();

 private:
    class FileAssetManager* asset_manager_;
};

wxIMPLEMENT_APP(PlanetEditor);