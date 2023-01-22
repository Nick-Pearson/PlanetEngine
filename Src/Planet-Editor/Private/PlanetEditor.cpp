#include "PlanetEditor.h"

#include "MainFrame.h"
#include "File/FileAssetManager.h"

bool PlanetEditor::OnInit()
{
    const char* path = std::getenv("PROJECT_DIR");
    P_ASSERT(path != nullptr, "Project path undefined");

    asset_manager_ = new FileAssetManager{path};

    MainFrame *frame = new MainFrame{asset_manager_};
    frame->Show(true);
    return true;
}