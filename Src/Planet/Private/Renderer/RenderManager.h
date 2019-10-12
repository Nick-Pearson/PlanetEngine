#pragma once

#include <memory>

#include "../Platform/Window.h"
#include "Renderer.h"

class UIRenderer;
class GPUResourceManager;
class CameraComponent;

class RenderManager
{
public:

	RenderManager(const Window* window);

	void RenderFrame();
	void SetCamera(std::shared_ptr<CameraComponent> camera);

	inline Renderer* GetRenderer() const { return mRenderer; }

private:

	std::shared_ptr<CameraComponent> mCamera;

	const Window* mWindow;
	Renderer* mRenderer;
	class UIRenderer* mUIRenderer;
};