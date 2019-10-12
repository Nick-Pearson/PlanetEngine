#include "RenderManager.h"

#include "D3D11/D3DRenderer.h"

RenderManager::RenderManager(const Window* window) :
	mWindow(window)
{
	mRenderer = new D3DRenderer{ *window };
}

void RenderManager::RenderFrame()
{
	mRenderer->Render(mCamera);
	mRenderer->SwapBuffers();
}

void RenderManager::SetCamera(std::shared_ptr<CameraComponent> camera)
{
	mCamera = camera;
}
