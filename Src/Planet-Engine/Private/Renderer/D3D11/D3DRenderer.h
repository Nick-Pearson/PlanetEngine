#pragma once

#include "Platform/PlanetWindows.h"
#include <d3d11.h>
#include <dxgidebug.h>
#include <DirectXMath.h>
#include <iostream>
#include <wrl/client.h>
#include "Container/LinkedList.h"
#include "../RenderState.h"
#include "../../PlanetLogging.h"
#include "Math/Vector.h"

#define d3dAssert( E ) { HRESULT r = (E); if(r != S_OK) { P_ERROR(TEXT("Err")) } }

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
			/*P_ERROR(Renderer, TEXT("D3D11 Error: %p "), msg->pDescription);*/ \
			delete rawmsg; \
		} \
	}

class D3DShader;
class Window;
class GPUResourceManager;
class ShaderManager;
class Mesh;
class CameraComponent;

struct WorldBufferData
{
	WorldBufferData() {}
	WorldBufferData(const Vector& inDir, float inStrength, const Vector& inCol) :
		sunDir(inDir), sunSkyStrength(inStrength), sunCol(inCol)
	{}

	Vector sunDir = Vector{ 0.0f, 1.0f, 0.0f };
	float sunSkyStrength = 20.0f;

	Vector sunCol = Vector{ 1.0f, 1.0f, 1.0f };
	char padding[4];
};

__declspec(align(16))
class D3DRenderer
{
	friend class D3DShader;

public:
	D3DRenderer(const Window& targetWindow,	Microsoft::WRL::ComPtr <ID3D11Device> mDevice, 
		Microsoft::WRL::ComPtr <IDXGISwapChain> mSwapChain, Microsoft::WRL::ComPtr <ID3D11DeviceContext> mContext );
	D3DRenderer(const D3DRenderer&) = delete;
	D3DRenderer& operator=(const D3DRenderer&) = delete;
	~D3DRenderer();

	// alignment for Direct X structures
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

	inline GPUResourceManager* GetResourceManager() const { return mResourceManager.get(); }

	RenderState* AddRenderState(const RenderState& state);
	void RemoveRenderState(const RenderState* state);

	void UpdateWorldBuffer(const WorldBufferData& data);

protected:

	void Draw(CameraComponent* component, const RenderState& state);

	void UpdateBuffer(Microsoft::WRL::ComPtr <ID3D11Buffer> buffer, void* bufferData, size_t bufferSize);

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

	Microsoft::WRL::ComPtr <ID3D11Buffer> mWorldPixelBuffer;
	WorldBufferData mWorldPixelBufferData;

	void CreateConstantBuffer(ID3D11Buffer** outBuffer, void* bufferPtr, size_t bufferSize);

private:
	
	// list of render commands
	// TODO: Replace with a heap?
	LinkedList<RenderState> renderStates;

	RenderState currentRenderState;

	std::shared_ptr <GPUResourceManager> mResourceManager;

	float aspectRatio;
};