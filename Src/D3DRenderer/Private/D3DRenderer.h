#pragma once

#include <d3d11.h>
#include <dxgidebug.h>
#include <wrl/client.h>
#include <iostream>
#include <memory>

#include "Platform/PlanetWindows.h"
#include <DirectXMath.h>
#include "Container/LinkedList.h"
#include "RenderState.h"
#include "Render/Renderer.h"
#include "Render/WorldBufferData.h"
#include "PlanetLogging.h"
#include "Math/Vector.h"

#define d3dAssert( E ) { HRESULT r = (E); if (r != S_OK) { P_ERROR(TEXT("Err")) } }

#define d3dFlushDebugMessages() \
    { \
        const auto len = mDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_D3D11); \
        for (auto i = 0; i < len; ++i) \
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
class ShaderManager;
class Mesh;
class CameraComponent;

__declspec(align(16))
class D3DRenderer : public Renderer
{
    friend class D3DShader;

 public:
    D3DRenderer(HWND window, Microsoft::WRL::ComPtr<ID3D11Device> mDevice,
        Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain, Microsoft::WRL::ComPtr<ID3D11DeviceContext> mContext);
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
    void Render(const CameraComponent& camera);

    RenderState* AddRenderState(const RenderState& state);
    void RemoveRenderState(const RenderState* state);

    void UpdateWorldBuffer(const WorldBufferData& data);

    void UpdateWindowSize(bool resize);

 protected:
    void Draw(const CameraComponent& camera, const RenderState& state);

    void UpdateBuffer(Microsoft::WRL::ComPtr <ID3D11Buffer> buffer, void* bufferData, size_t bufferSize);

 private:
    // D3D11 Ptrs
    Microsoft::WRL::ComPtr <ID3D11Device> mDevice;
    Microsoft::WRL::ComPtr <IDXGISwapChain> mSwapChain;
    Microsoft::WRL::ComPtr <ID3D11DeviceContext> mContext;
    Microsoft::WRL::ComPtr <ID3D11RenderTargetView> mTarget;
    Microsoft::WRL::ComPtr <ID3D11DepthStencilView> mDepthStencilView;
    Microsoft::WRL::ComPtr <IDXGIInfoQueue> mDxgiInfoQueue;

    Microsoft::WRL::ComPtr<ID3D11BlendState> mAlphaBlendState;
    Microsoft::WRL::ComPtr<ID3D11BlendState> mNoAlphaBlendState;

    ID3D11RasterizerState* WireFrame;
    ID3D11RasterizerState* Solid;

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

    float aspectRatio;
};