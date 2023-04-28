#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <string>

#include "PlanetEngine.h"

class PlanetEditor: public wxApp
{
 public:
    virtual bool OnInit();

 private:
    class FileAssetManager* asset_manager_;
};

wxIMPLEMENT_APP(PlanetEditor);