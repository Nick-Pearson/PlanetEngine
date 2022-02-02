#pragma once

#include <d3d12.h>
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

__declspec(align(16))
class D3DRenderer : public Renderer
{
    friend class D3DShader;

 public:
    D3DRenderer(ID3D12GraphicsCommandList* command_list);
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

    void BindRenderTarget(const RenderTarget* target);
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

    // void UpdateBuffer(wrl::ComPtr<ID3D11Buffer> buffer, void* bufferData, size_t bufferSize);

 private:
    bool render_solid_ = true;
    bool render_wireframe_ = false;

    ID3D12GraphicsCommandList* command_list_;

    const RenderTarget* render_target_ = nullptr;

    // standard vertex shader, later will be specified based on which vertex attributes a mesh has
    std::shared_ptr<D3DVertexShader> vertexShader;

    // Constant Buffers
    // wrl::ComPtr<ID3D12Buffer> mSlowConstantBuffer;
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

    // wrl::ComPtr<ID3D11Buffer> mFastConstantBuffer;
    struct FastConstantBuffer
    {
        FastConstantBuffer()
        {
            model = DirectX::XMMatrixIdentity();
        }
        DirectX::XMMATRIX model;
    };
    FastConstantBuffer mFastConstantBufferData;

    // wrl::ComPtr<ID3D11Buffer> mWorldPixelBuffer;
    WorldBufferData mWorldPixelBufferData;

    // void CreateConstantBuffer(ID3D11Buffer** outBuffer, void* bufferPtr, size_t bufferSize);

 private:
    // list of render commands
    // TODO: Replace with a heap?
    LinkedList<RenderState> renderStates;

    RenderState currentRenderState;
    unsigned int used_texture_slots_ = 0;

    float aspect_ratio_ = 1.0f;

    std::shared_ptr<GPUMaterialHandle> wireframe_shader_;
};