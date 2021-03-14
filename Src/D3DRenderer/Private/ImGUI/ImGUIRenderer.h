#pragma once

#include <d3d11.h>
#include <wrl/client.h>

class ImGUIRenderer
{
 public:
    ImGUIRenderer(HWND window, Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
    ~ImGUIRenderer();

    void Render();
    void NewFrame();
};