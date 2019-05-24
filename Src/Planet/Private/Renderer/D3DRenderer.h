#pragma once

#include "../Platform/PlanetWindows.h"
#include <d3d11.h>
#include <dxgidebug.h>
#include <iostream>

#define d3dAssert( E ) { HRESULT r = (E); if(r != S_OK) { std::cout << "Error : " << r << std::endl; } }

#define d3dFlushDebugMessages() \
	{ \
		const auto len = mDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_D3D11); \
		for( auto i = 0; i < len; ++i) \
		{ \
			SIZE_T msg_len; \
			mDxgiInfoQueue->GetMessage(DXGI_DEBUG_D3D11, i, nullptr, &msg_len); \
			char* rawmsg = new char[msg_len]; \
			auto msg = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(rawmsg); \
			mDxgiInfoQueue->GetMessage(DXGI_DEBUG_D3D11, i, msg, &msg_len); \
			std::cout << msg->pDescription << std::endl; \
			delete rawmsg; \
		} \
	}

class D3DShader;
class Window;
class MeshManager;
class Mesh;

class D3DRenderer
{
	friend class D3DShader;

public:
	D3DRenderer( const Window& targetWindow );
	D3DRenderer(const D3DRenderer&) = delete;
	D3DRenderer& operator=(const D3DRenderer&) = delete;
	~D3DRenderer();

	// Renderer Interface
	void SwapBuffers();

	void Render();
	void RenderMesh(std::shared_ptr<Mesh> mesh);

	inline MeshManager* GetMeshManager() const { return mMeshManager; }

private:
	ID3D11Device* mDevice = nullptr;
	IDXGISwapChain* mSwapChain = nullptr;
	ID3D11DeviceContext* mContext = nullptr;
	ID3D11RenderTargetView* mTarget = nullptr;
	IDXGIInfoQueue* mDxgiInfoQueue = nullptr;

	D3DShader* vertexShader = nullptr;
	D3DShader* pixelShader = nullptr;

	MeshManager* mMeshManager = nullptr;
};