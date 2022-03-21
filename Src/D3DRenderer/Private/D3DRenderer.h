#pragma once

#include <d3d12.h>
#include <iostream>
#include <memory>
#include <vector>
#include <DirectXMath.h>

#include "Platform/PlanetWindows.h"
#include "Container/LinkedList.h"
#include "Render/Renderer.h"
#include "Render/WorldBufferData.h"
#include "PlanetLogging.h"
#include "Math/Vector.h"
#include "World/CameraComponent.h"

#include "Target/RenderTarget.h"
#include "RenderState.h"
#include "D3DConstants.h"
#include "D3DAssert.h"

__declspec(align(16))
class D3DRenderer : public Renderer
{
    friend class D3DShader;

 public:
    D3DRenderer(ID3D12Device2* device, ID3D12GraphicsCommandList* command_list, const D3DRootSignature* root_signature);
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

    void AddRenderState(const RenderState& state);

    void UpdateWorldBuffer(const WorldBufferData& data);

    void RenderDebugUI();

 protected:
    void Draw(const CameraComponent& camera, const RenderState& state, bool use_materials);

    void PreRender(const CameraComponent& camera);
    void PostRender();

    void UpdateWorldMatrix(const CameraComponent& camera, bool use_world_matrix);
    // void UpdateBuffer(wrl::ComPtr<ID3D11Buffer> buffer, void* bufferData, size_t bufferSize);

 private:
    bool render_solid_ = true;
    bool render_wireframe_ = false;

    ID3D12Device2* device_;
    ID3D12GraphicsCommandList* command_list_;
    const D3DRootSignature* const root_signature_;

    const RenderTarget* render_target_ = nullptr;

int count = 0;

    D3DSlowVSConstants::Data slow_constants_;
    D3DFastVSConstants::Data fast_constants_;

    // wrl::ComPtr<ID3D11Buffer> mWorldPixelBuffer;
    WorldBufferData mWorldPixelBufferData;

    // void CreateConstantBuffer(ID3D11Buffer** outBuffer, void* bufferPtr, size_t bufferSize);

 private:
    // list of render commands
    // TODO: Replace with a heap?
    std::vector<RenderState> render_states_;

    float aspect_ratio_ = 1.0f;

    // std::shared_ptr<GPUMaterialHandle> wireframe_shader_;
};