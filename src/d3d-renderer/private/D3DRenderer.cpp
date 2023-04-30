#include "D3DRenderer.h"

#include <wrl/client.h>
#include <D3Dcompiler.h>
#include <memory>
#include <vector>

#include "d3dx12.h"

#include "Platform/Window.h"
#include "Texture/D3DTexture.h"
#include "GPUResourceManager.h"
#include "Mesh/Mesh.h"
#include "World/CameraComponent.h"
#include "Math/Transform.h"
#include "imgui.h"

D3DRenderer::D3DRenderer(ID3D12Device2* device, ID3D12GraphicsCommandList* command_list, const BaseRootSignature* root_signature, SRVHeap* srv_heap) :
    device_(device), command_list_(command_list), root_signature_(root_signature), srv_heap_(srv_heap)
{
    device_->AddRef();
    command_list_->AddRef();
}

D3DRenderer::~D3DRenderer()
{
    device_->Release();
    command_list_->Release();
}

void D3DRenderer::BindRenderTarget(const RenderTarget* target)
{
    render_target_ = target;

    D3D12_VIEWPORT viewport;
    viewport.Width = static_cast<float>(render_target_->GetWidth());
    viewport.Height = static_cast<float>(render_target_->GetHeight());
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    command_list_->RSSetViewports(1, &viewport);

    D3D12_RECT scissor_rect = CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX);
    command_list_->RSSetScissorRects(1, &scissor_rect);

    aspect_ratio_ = viewport.Height / viewport.Width;

    P_LOG("Set render size to {}x{}", target->GetWidth(), target->GetHeight());
}

void D3DRenderer::UnbindRenderTarget()
{
    render_target_ = nullptr;
}

void D3DRenderer::Render(const CameraComponent& camera)
{
    PreRender(camera);

    if (render_solid_)
    {
        // draw each state
        for (const RenderState& state : render_states_)
        {
            Draw(camera, state, true);
        }
    }

    PostRender();
}

void D3DRenderer::PreRender(const CameraComponent& camera)
{
    auto command_allocator = render_target_->GetCommandAllocator();

    command_allocator->Reset();
    d3dAssert(command_list_->Reset(command_allocator, nullptr));

    auto back_buffer = render_target_->GetRenderTarget();
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(back_buffer->resource_,
        D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    command_list_->ResourceBarrier(1, &barrier);
    const int period = 10000;
    const float colour[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    command_list_->ClearRenderTargetView(back_buffer->cpu_handle_, colour, 0, nullptr);

    auto depth_stencil = render_target_->GetDepthStencil();
    command_list_->ClearDepthStencilView(depth_stencil->cpu_handle_, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    command_list_->OMSetRenderTargets(1u, &back_buffer->cpu_handle_, false, &depth_stencil->cpu_handle_);

    D3D12_VIEWPORT viewport;
    viewport.Width = static_cast<float>(render_target_->GetWidth());
    viewport.Height = static_cast<float>(render_target_->GetHeight());
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    command_list_->RSSetViewports(1, &viewport);

    D3D12_RECT scissor_rect = CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX);
    command_list_->RSSetScissorRects(1, &scissor_rect);
    // setup projection matrix
    slow_constants_.view_ = DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveLH(1.0f, aspect_ratio_, camera.NearClip, camera.FarClip));

    srv_heap_->Bind(command_list_);
}

void D3DRenderer::PostRender()
{
}

void D3DRenderer::AddRenderState(const RenderState& state)
{
    render_states_.push_back(state);
}

void D3DRenderer::UpdateWorldBuffer(const WorldBufferData& data)
{
    fast_constants_.sun_dir_ = data.sunDir.ToVector3Reg();
    fast_constants_.sun_sky_strength_ = data.sunSkyStrength;
    fast_constants_.sun_col_ = data.sunCol.ToVector3Reg();
}

void D3DRenderer::RenderDebugUI()
{
    ImGui::Checkbox("Draw Solid", &render_solid_);
    ImGui::Checkbox("Draw Wireframe", &render_wireframe_);
}

void D3DRenderer::Draw(const CameraComponent& camera, const RenderState& state, bool use_materials)
{
    if (!state.IsValid()) return;

    state.root_signature_->Bind(command_list_);

    UpdateWorldMatrix(camera, state.use_world_matrix_);

    DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(state.model_.GetMatrix());
    fast_constants_.model_ = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, state.model_.GetMatrix()));
    command_list_->SetGraphicsRoot32BitConstants(1, D3DFastConstants::size_32_bit_, &fast_constants_, 0);

    command_list_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    command_list_->IASetVertexBuffers(0u, 1u, state.mesh_->GetVertexBuffer());
    command_list_->IASetIndexBuffer(state.mesh_->GetTriangleBuffer());

    state.material_->Bind(command_list_);
    state.pipeline_state_->Bind(command_list_);

    const auto instance_count = 1u;
    const auto start_index = 0u;
    const auto start_vertex = 0u;
    const auto start_instance = 0u;
    command_list_->DrawIndexedInstanced(state.mesh_->GetTriangleCount(), instance_count, start_index, start_vertex, start_instance);
}

void D3DRenderer::UpdateWorldMatrix(const CameraComponent& camera, bool use_world_matrix)
{
    Transform camera_transform = camera.GetWorldTransform();
    if (!use_world_matrix)
    {
        camera_transform.location = Vector{};
        camera_transform.scale = Vector{1.0f, 1.0f, 1.0f};
    }
    DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(camera_transform.GetMatrix());
    slow_constants_.world_ = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, camera_transform.GetMatrix()));

    command_list_->SetGraphicsRoot32BitConstants(0, D3DSlowVSConstants::size_32_bit_, &slow_constants_, 0);
}