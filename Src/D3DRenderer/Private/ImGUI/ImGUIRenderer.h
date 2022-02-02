#pragma once

#include "Platform/PlanetWindows.h"

class ImGUIRenderer
{
 public:
    explicit ImGUIRenderer(HWND window);
    ~ImGUIRenderer();

    void Render();
    void NewFrame();
};