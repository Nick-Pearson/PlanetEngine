#include "PlanetEditor.h"

#include "IconsFontAwesome4.h"

#include "AssetViewer/AssetViewer.h"
#include "FileAssetManager.h"

PlanetEditor::PlanetEditor(PlanetEngine* engine, const std::string& project_path) :
    engine_(engine), project_path_(project_path)
{
    engine_->game_update_.AddListener([=] (float delta_time) {
        this->Draw();
    });


    ImGuiIO& io = ImGui::GetIO();
    editor_font_ = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/calibri.ttf", 18.0f);

    ImFontConfig config;
    config.MergeMode = true;
    config.GlyphMinAdvanceX = 13.0f;
    static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    io.Fonts->AddFontFromFileTTF("./fontawesome-webfont.ttf", 16.0f, &config, icon_ranges);

    io.Fonts->Build();

    asset_manager_ = new FileAssetManager{nullptr, project_path};
    asset_viewer_ = new AssetViewer{asset_manager_};
}

void PlanetEditor::Draw()
{
    ImGui::PushFont(editor_font_);

    ImGui::Begin("Assets");
    asset_viewer_->Draw();
    ImGui::End();

    ImGui::PopFont();
}