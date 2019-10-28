#pragma once

#include <d3d11.h>
#include <wrl/client.h>

class Window;

class UIRenderer
{
public:
	UIRenderer(const Window* window, Microsoft::WRL::ComPtr <ID3D11Device> device, Microsoft::WRL::ComPtr <ID3D11DeviceContext> context);
	~UIRenderer();

	void Render();
	void NewFrame();
};