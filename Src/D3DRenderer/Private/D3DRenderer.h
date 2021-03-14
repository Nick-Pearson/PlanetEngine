#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <iostream>
#include <memory>
#include <DirectXMath.h>

#include "Platform/PlanetWindows.h"
#include "Container/LinkedList.h"
#include "Render/Renderer.h"
#include "Render/WorldBufferData.h"
#include "PlanetLogging.h"
#include "Math/Vector.h"
#include "World/CameraComponent.h"

#include "Shader/D3DShaderLoader.h"
#include "RenderState.h"
#include "D3DAssert.h"

namespace wrl = Microsoft::WRL;

__declspec(align(16))
class D3DRenderer : public Renderer
{
    friend class D3DShader;

 public:
    D3DRenderer(HWND window,
        wrl::ComPtr<ID3D11Device> mDevice,
        wrl::ComPtr<IDXGISwapChain> mSwapChain,
        wrl::ComPtr<ID3D11DeviceContext> mContext);
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

    void UpdateBuffer(wrl::ComPtr<ID3D11Buffer> buffer, void* bufferData, size_t bufferSize);

 private:
    // D3D11 Ptrs
    wrl::ComPtr<ID3D11Device> mDevice;
    wrl::ComPtr<IDXGISwapChain> mSwapChain;
    wrl::ComPtr<ID3D11DeviceContext> mContext;
    wrl::ComPtr<ID3D11RenderTargetView> mTarget;
    wrl::ComPtr<ID3D11DepthStencilView> mDepthStencilView;

    wrl::ComPtr<ID3D11BlendState> mAlphaBlendState;
    wrl::ComPtr<ID3D11BlendState> mNoAlphaBlendState;

    ID3D11RasterizerState* WireFrame;
    ID3D11RasterizerState* Solid;

    // standard vertex shader, later will be specified based on which vertex attributes a mesh has
    std::shared_ptr<D3DVertexShader> vertexShader;

    // Constant Buffers
    wrl::ComPtr<ID3D11Buffer> mSlowConstantBuffer;
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

    wrl::ComPtr<ID3D11Buffer> mFastConstantBuffer;
    struct FastConstantBuffer
    {
        FastConstantBuffer()
        {
            model = DirectX::XMMatrixIdentity();
        }
        DirectX::XMMATRIX model;
    };
    FastConstantBuffer mFastConstantBufferData;

    wrl::ComPtr<ID3D11Buffer> mWorldPixelBuffer;
    WorldBufferData mWorldPixelBufferData;

    void CreateConstantBuffer(ID3D11Buffer** outBuffer, void* bufferPtr, size_t bufferSize);

 private:
    // list of render commands
    // TODO: Replace with a heap?
    LinkedList<RenderState> renderStates;

    RenderState currentRenderState;

    float aspectRatio;
};