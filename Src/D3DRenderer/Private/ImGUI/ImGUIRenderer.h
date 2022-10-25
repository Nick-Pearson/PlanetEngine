#pragma once

#include "Platform/PlanetWindows.h"
#include "Descriptor/SRVHeap.h"

class ImGUIRenderer
{
 public:
    explicit ImGUIRenderer(HWND window, ID3D12Device2* device, SRVHeap* srv_heap, ID3D12GraphicsCommandList* command_list);
    ~ImGUIRenderer();

    void Render();
    void NewFrame();

 private:
    ID3D12GraphicsCommandList* const command_list_;
};