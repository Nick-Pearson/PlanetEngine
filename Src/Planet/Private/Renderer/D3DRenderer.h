#pragma once

#include "../Platform/PlanetWindows.h"
#include <d3d11.h>

class Window;

class D3DRenderer
{
public:
	D3DRenderer( const Window& targetWindow );
	D3DRenderer(const D3DRenderer&) = delete;
	D3DRenderer& operator=(const D3DRenderer&) = delete;
	~D3DRenderer();

	// Renderer Interface
	void SwapBuffers();

private:
	ID3D11Device* mDevice = nullptr;
	IDXGISwapChain* mSwapChain = nullptr;
	ID3D11DeviceContext* mContext = nullptr;
	ID3D11RenderTargetView* mTarget = nullptr;

};