#include "D3DRenderer.h"

#include "../Platform/Window.h"

#pragma comment(lib,"d3d11.lib")

D3DRenderer::D3DRenderer(const Window& targetWindow)
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = targetWindow.GetWindowHandle();
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	// create device and front/back buffers, and swap chain and rendering context
	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&mSwapChain,
		&mDevice,
		nullptr,
		&mContext
	);

	ID3D11Resource* BackBuffer = nullptr;
	mSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), (void**)&BackBuffer);
	mDevice->CreateRenderTargetView(BackBuffer, nullptr, &mTarget);
	BackBuffer->Release();
}

D3DRenderer::~D3DRenderer()
{
	if (mContext)
		mContext->Release();
	
	if (mSwapChain)
		mSwapChain->Release();
	
	if (mDevice)
		mDevice->Release();
}

void D3DRenderer::SwapBuffers()
{
	mSwapChain->Present(1u, 0u);

	const float colour[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	mContext->ClearRenderTargetView(mTarget, colour);
}
