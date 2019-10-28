#include "RenderManager.h"

#include "D3D11/D3DRenderer.h"
#include "UIRenderer.h"

RenderManager::RenderManager(const Window* window) :
	mWindow(window)
{
	InitD3D11Device(window);
	mRenderer = new D3DRenderer{ *window, mDevice, mSwapChain, mContext };
	mUIRenderer = new UIRenderer{ window, mDevice, mContext };
}

RenderManager::~RenderManager()
{
	delete mRenderer;
	delete mUIRenderer;
}

void RenderManager::RenderFrame()
{
	mRenderer->Render(mCamera);
	mUIRenderer->Render();
	mRenderer->SwapBuffers();
	mUIRenderer->NewFrame();
}

void RenderManager::SetCamera(std::shared_ptr<CameraComponent> camera)
{
	mCamera = camera;
}

void RenderManager::InitD3D11Device(const Window* targetWindow)
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
	sd.OutputWindow = targetWindow->GetWindowHandle();
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	// create device and front/back buffers, and swap chain and rendering context
	unsigned int createFlags = D3D11_CREATE_DEVICE_DEBUG;

	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		mSwapChain.GetAddressOf(),
		mDevice.GetAddressOf(),
		nullptr,
		mContext.GetAddressOf()
	);
}
