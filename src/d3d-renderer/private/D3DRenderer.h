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
#include "BaseRootSignature.h"
#include "RenderState.h"
#include "D3DConstants.h"
#include "Descriptor/SRVHeap.h"
#include "D3DAssert.h"

__declspec(align(16))
class D3DRenderer : public Renderer
{
    friend class D3DShader;

 public:
    D3DRenderer(ID3D12Device2* device, ID3D12GraphicsCommandList* command_list, const BaseRootSignature* root_signature, SRVHeap* srv_heap);
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

 private:
    bool render_solid_ = true;
    bool render_wireframe_ = false;

    ID3D12Device2* device_;
    ID3D12GraphicsCommandList* command_list_;
    const BaseRootSignature* const root_signature_;
    SRVHeap* const srv_heap_;

    const RenderTarget* render_target_ = nullptr;

    D3DSlowVSConstants::Data slow_constants_;
    D3DFastVSConstants::Data fast_constants_;

    D3DWorldPSConstants::Data world_constants_;

 private:
    // list of render commands
    // TODO: Replace with a heap?
    std::vector<RenderState> render_states_;

    float aspect_ratio_ = 1.0f;
};