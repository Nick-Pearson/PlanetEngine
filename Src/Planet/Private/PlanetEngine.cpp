#include "PlanetEngine.h"

#include "Platform/Window.h"
#include "Renderer/Renderer.h"
#include "Mesh/Mesh.h"

#include <memory>
#include "Mesh/OBJImporter.h"

PlanetEngine::PlanetEngine()
{
}

PlanetEngine::~PlanetEngine()
{

}

PlanetEngine* PlanetEngine::Get()
{
	static PlanetEngine* sEngine = nullptr;

	if (!sEngine) sEngine = new PlanetEngine{};
	return sEngine;
}

void PlanetEngine::Run()
{
	Window window{1280, 720};
	window.SetWindowName("PlanetEngine");

	Renderer renderer{ window };

	Vertex v[] = {
		Vector{ -1.0f, -1.0f, 1.0f },
		Vector{ 1.0f,  -1.0f, 1.0f },
		Vector{ -1.0f, -1.0f, -1.0f },
		Vector{ 1.0f,  -1.0f, -1.0f },
		Vector{ -1.0f, 1.0f, 1.0f },
		Vector{ 1.0f,  1.0f, 1.0f },
		Vector{ -1.0f, 1.0f, -1.0f },
		Vector{ 1.0f,  1.0f, -1.0f },
	};
	unsigned short t[] = { 
		// front
		0,1,5,
		0,5,4,

		// back 2,3,6,7
		3,2,6,
		3,6,7,

		// top
		6,4,5,
		6,5,7,

		// bottom 0,1,2,3
		0,3,1,
		0,2,3,

		//right
		5,1,3,
		5,3,7,

		// left 6,4,2,0
		0,6,2,
		0,4,6
	};
	std::shared_ptr<Mesh> cube = std::make_shared<Mesh>(v, 8, t, 36);
	cube->RecalculateNormals();

	std::shared_ptr<Mesh> model = OBJImporter::Import("Assets/Models/teapot.obj");

	renderer.GetMeshManager()->LoadMesh(model);
	//renderer.GetMeshManager()->LoadMesh(cube);

	ExitCode = -1;
	while (ExitCode == -1)
	{
#if PLATFORM_WIN
		PumpWindowsMessages();
#endif
		//ProcessInput();
		//UpdateGameplay();
		
		//renderer.RenderMesh(cube);
		renderer.RenderMesh(model);
		renderer.SwapBuffers();
	}

	//renderer.GetMeshManager()->UnloadMesh(cube);
	renderer.GetMeshManager()->UnloadMesh(model);
}

#if PLATFORM_WIN
void PlanetEngine::PumpWindowsMessages()
{
	MSG msg;
	BOOL result;

	while ((result = PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) != 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

LRESULT CALLBACK PlanetEngine::ProcessWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		ExitCode = 0;
		return 0;
	case WM_QUIT:
		ExitCode = wParam;
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
#endif