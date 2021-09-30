#pragma once

#include <string>

#include "imgui.h"

#include "PlanetEngine.h"

class PlanetEditor
{
 public:
    explicit PlanetEditor(PlanetEngine* engine, const std::string& project_path);

 private:
    void Draw();

    std::string project_path_;
    PlanetEngine* engine_;

    class FileAssetManager* asset_manager_;
    class AssetViewer* asset_viewer_;

    ImFont* editor_font_;
};