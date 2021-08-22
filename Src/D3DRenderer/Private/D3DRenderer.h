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

#include "Target/RenderTarget.h"
#include "Shader/D3DShaderLoader.h"
#include "RenderState.h"
#include "D3DAssert.h"

namespace wrl = Microsoft::WRL;

__declspec(align(16))
class D3DRenderer : public Renderer
{
    friend class D3DShader;

 public:
    D3DRenderer(
        wrl::ComPtr<ID3D11Device> mDevice,
        wrl::ComPtr<IDXGISwapChain> mSwapChain,
        wrl::ComPtr<ID3D11DeviceContext> mContext,
        std::shared_ptr<GPUMaterialHandle> wireframe_shader);
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

    void BindRenderTarget(const RenderTarget& target);
    void UnbindRenderTarget();

    // renders a particular camera
    void Render(const CameraComponent& camera);

    RenderState* AddRenderState(const RenderState& state);
    void RemoveRenderState(const RenderState* state);

    void UpdateWorldBuffer(const WorldBufferData& data);

    void RenderDebugUI();

 protected:
    void Draw(const CameraComponent& camera, const RenderState& state, bool use_materials);

    void PreRender(const CameraComponent& camera);
    void PostRender();

    void UpdateBuffer(wrl::ComPtr<ID3D11Buffer> buffer, void* bufferData, size_t bufferSize);

 private:
    bool render_solid_ = true;
    bool render_wireframe_ = false;

    // D3D11 Ptrs
    wrl::ComPtr<ID3D11Device> mDevice;
    wrl::ComPtr<IDXGISwapChain> mSwapChain;
    wrl::ComPtr<ID3D11DeviceContext> mContext;
    ID3D11RenderTargetView* render_target_view_ = nullptr;
    ID3D11DepthStencilView* depth_stencil_view_ = nullptr;

    wrl::ComPtr<ID3D11BlendState> mAlphaBlendState;
    wrl::ComPtr<ID3D11BlendState> mNoAlphaBlendState;

    wrl::ComPtr<ID3D11DepthStencilState> use_depth_stencil_state_;
    wrl::ComPtr<ID3D11DepthStencilState> no_depth_stencil_state_;

    ID3D11RasterizerState* solid_state_ = nullptr;
    ID3D11RasterizerState* wire_frame_state_ = nullptr;
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
    unsigned int used_texture_slots_ = 0;

    float aspect_ratio_ = 1.0f;

    std::shared_ptr<GPUMaterialHandle> wireframe_shader_;
};