#pragma once

#include <memory>

#include <d3d11.h>
#include <wrl/client.h>

#include "../Platform/Window.h"
#include "Renderer.h"

class UIRenderer;
class GPUResourceManager;
class CameraComponent;

class RenderManager
{
public:

	RenderManager(const Window* window);
	~RenderManager();

	void RenderFrame();
	void SetCamera(std::shared_ptr<CameraComponent> camera);

	inline Renderer* GetRenderer() const { return mRenderer; }

private:

	void RenderDebugUI();

	void InitD3D11Device(const Window* targetWindow);

private:

	Microsoft::WRL::ComPtr <ID3D11Device> mDevice;
	Microsoft::WRL::ComPtr <IDXGISwapChain> mSwapChain;
	Microsoft::WRL::ComPtr <ID3D11DeviceContext> mContext;

	std::shared_ptr<CameraComponent> mCamera;

	const Window* mWindow;
	Renderer* mRenderer;
	class UIRenderer* mUIRenderer;

	long long lastFrameMS = 1;
};