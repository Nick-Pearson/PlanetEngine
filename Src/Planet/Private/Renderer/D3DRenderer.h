#pragma once

#include "../Platform/PlanetWindows.h"
#include <d3d11.h>
#include <dxgidebug.h>
#include <DirectXMath.h>
#include <iostream>
#include <wrl/client.h>
#include "../Container/LinkedList.h"
#include "RenderState.h"

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
class GPUResourceManager;
class ShaderManager;
class Mesh;
class CameraComponent;

__declspec(align(16))
class D3DRenderer
{
	friend class D3DShader;

public:
	D3DRenderer( const Window& targetWindow );
	D3DRenderer(const D3DRenderer&) = delete;
	D3DRenderer& operator=(const D3DRenderer&) = delete;
	~D3DRenderer();

	void* operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

	void operator delete(void* p)
	{
		_mm_free(p);
	}

	// Renderer Interface
	void SwapBuffers();

	// renders a particular camera
	void Render(std::shared_ptr<CameraComponent> camera);

	inline GPUResourceManager* GetResourceManager() const { return mMeshManager.get(); }

	RenderState* AddRenderState(const RenderState& state);
	void RemoveRenderState(const RenderState* state);

protected:

	void Draw(CameraComponent* component, const RenderState& state);

private:
	// D3D11 Ptrs
	Microsoft::WRL::ComPtr <ID3D11Device> mDevice;
	Microsoft::WRL::ComPtr <IDXGISwapChain> mSwapChain;
	Microsoft::WRL::ComPtr <ID3D11DeviceContext> mContext;
	Microsoft::WRL::ComPtr <ID3D11RenderTargetView> mTarget;
	Microsoft::WRL::ComPtr <ID3D11DepthStencilView> mDepthStencilView;
	Microsoft::WRL::ComPtr <IDXGIInfoQueue> mDxgiInfoQueue;

	// standard vertex shader, later will be specified based on which vertex attributes a mesh has
	std::shared_ptr<D3DShader> vertexShader;

	// pixel shader used when the specified shader is invalid, displays object in bright pink
	std::shared_ptr <D3DShader> invalidShader;

	// Constant Buffers
	Microsoft::WRL::ComPtr <ID3D11Buffer> mSlowConstantBuffer;
	struct SlowConstantBuffer
	{
		SlowConstantBuffer()
		{
			world = DirectX::XMMatrixIdentity();
			view = DirectX::XMMatrixIdentity();
		}

		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
	};
	SlowConstantBuffer mSlowConstantBufferData;

	Microsoft::WRL::ComPtr <ID3D11Buffer> mFastConstantBuffer;
	struct FastConstantBuffer
	{
		FastConstantBuffer()
		{
			model = DirectX::XMMatrixIdentity();
		}
		DirectX::XMMATRIX model;
	};
	FastConstantBuffer mFastConstantBufferData;

	void CreateConstantBuffer(ID3D11Buffer** outBuffer, void* bufferPtr, size_t bufferSize);

private:
	
	// list of render commands
	// TODO: Replace with a heap?
	LinkedList<RenderState> renderStates;

	RenderState currentRenderState;

	std::shared_ptr <GPUResourceManager> mMeshManager;

	float aspectRatio;
};