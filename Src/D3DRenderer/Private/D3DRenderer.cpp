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

    // // apply depth buffer
    // D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    // dsDesc.DepthEnable = TRUE;
    // dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    // dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    // d3dAssert(mDevice->CreateDepthStencilState(&dsDesc, use_depth_stencil_state_.GetAddressOf()));
    // dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    // dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
    // d3dAssert(mDevice->CreateDepthStencilState(&dsDesc, no_depth_stencil_state_.GetAddressOf()));

    // // bind depth state
    // mContext->OMSetDepthStencilState(use_depth_stencil_state_.Get(), 1u);

    // vertexShader->Use(mContext.Get());

    // CreateConstantBuffer(&mSlowConstantBuffer, &mSlowConstantBufferData, sizeof(mSlowConstantBufferData));
    // CreateConstantBuffer(&mFastConstantBuffer, &mFastConstantBufferData, sizeof(mFastConstantBufferData));

    // ID3D11Buffer* VertBuffers[2] = { mSlowConstantBuffer.Get(), mFastConstantBuffer.Get() };
    // mContext->VSSetConstantBuffers(0u, 2u, VertBuffers);

    // CreateConstantBuffer(&mWorldPixelBuffer, &mWorldPixelBufferData, sizeof(mWorldPixelBufferData));

    // ID3D11Buffer* PixBuffers[1] = { mWorldPixelBuffer.Get() };
    // mContext->PSSetConstantBuffers(0u, 1u, PixBuffers);

    // {
    //     D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC{ CD3D11_DEFAULT{} };
    //     auto& brt = blendDesc.RenderTarget[0];
    //     brt.BlendEnable = TRUE;
    //     brt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
    //     brt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    //     brt.BlendOp = D3D11_BLEND_OP_ADD;
    //     brt.SrcBlendAlpha = D3D11_BLEND_ZERO;
    //     brt.DestBlendAlpha = D3D11_BLEND_ZERO;
    //     brt.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    //     brt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    //     d3dAssert(mDevice->CreateBlendState(&blendDesc, &mAlphaBlendState));
    // }
    // {
    //     D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC{ CD3D11_DEFAULT{} };
    //     auto& brt = blendDesc.RenderTarget[0];
    //     brt.BlendEnable = FALSE;
    //     brt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    //     d3dAssert(mDevice->CreateBlendState(&blendDesc, &mNoAlphaBlendState));
    // }
    // mContext->OMSetBlendState(mNoAlphaBlendState.Get(), nullptr, 0xFFFFFFFFu);

    // D3D11_RASTERIZER_DESC state_desc = {};
    // state_desc.FillMode = D3D11_FILL_SOLID;
    // state_desc.CullMode = D3D11_CULL_BACK;
    // state_desc.DepthClipEnable = true;
    // d3dAssert(mDevice->CreateRasterizerState(&state_desc, &solid_state_));
    // state_desc.CullMode = D3D11_CULL_NONE;
    // state_desc.FillMode = D3D11_FILL_WIREFRAME;
    // state_desc.DepthClipEnable = false;
    // d3dAssert(mDevice->CreateRasterizerState(&state_desc, &wire_frame_state_));
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

    // sort render states
    // use array sort for better cache performance in the sort and the subsequent loop
    // std::sort(render_states_.begin(), render_states_.end());

    if (render_solid_)
    {
        // mContext->RSSetState(solid_state_);
        // draw each state
        for (const RenderState& state : render_states_)
        {
            Draw(camera, state, true);
        }
    }

    // if (render_wireframe_)
    // {
    //     mContext->RSSetState(wire_frame_state_);

    //     currentRenderState.material = wireframe_shader_;
    //     wireframe_shader_->shader->Use(mContext.Get());
    //     mContext->OMSetBlendState(mNoAlphaBlendState.Get(), nullptr, 0xFFFFFFFFu);

    //     // draw each state
    //     for (const RenderState& state : sortedStates)
    //     {
    //         Draw(camera, state, false);
    //     }
    // }

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
    const float colour[4] = { ((count++ % period) / static_cast<float>(period)), 0.5f, 0.5f, 1.0f };
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
    mWorldPixelBufferData = data;
    // UpdateBuffer(mWorldPixelBuffer, &mWorldPixelBufferData, sizeof(mWorldPixelBufferData));
}

void D3DRenderer::RenderDebugUI()
{
    ImGui::Checkbox("Draw Solid", &render_solid_);
    ImGui::Checkbox("Draw Wireframe", &render_wireframe_);
}

void D3DRenderer::Draw(const CameraComponent& camera, const RenderState& state, bool use_materials)
{
    if (!state.IsValid()) return;

    // // apply the render state
    // if (currentRenderState.UseDepthBuffer != state.UseDepthBuffer)
    // {
    //     if (state.UseDepthBuffer)
    //     {
    //         mContext->OMSetDepthStencilState(use_depth_stencil_state_.Get(), 1u);
    //     }
    //     else
    //     {
    //         mContext->OMSetDepthStencilState(no_depth_stencil_state_.Get(), 1u);
    //     }
    // }


    state.material_->GetRootSignature()->Bind(command_list_);

    UpdateWorldMatrix(camera, state.use_world_matrix_);

    DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(state.model_.GetMatrix());
    fast_constants_.model_ = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, state.model_.GetMatrix()));
    command_list_->SetGraphicsRoot32BitConstants(1, D3DFastVSConstants::size_32_bit_, &fast_constants_, 0);

    // bind textures
    // command_list_->SetGraphicsRootDescriptorTable();

    command_list_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    command_list_->IASetVertexBuffers(0u, 1u, state.mesh_->GetVertexBuffer());
    command_list_->IASetIndexBuffer(state.mesh_->GetTriangleBuffer());

    state.material_->Bind(command_list_);

    // command_list_->SetGraphicsRootShaderResourceView()

    // if (use_materials && currentRenderState.material != state.material)
    // {
    //     state.material->shader->Use(mContext.Get());
    //     unsigned int num_textures = state.material->textures.size();
    //     for (unsigned int i = 0; i < num_textures; ++i)
    //     {
    //         state.material->textures[i]->Use(mContext.Get(), i);
    //     }
    //     used_texture_slots_ = num_textures > used_texture_slots_ ? num_textures : used_texture_slots_;

    //     if (state.material->alpha)
    //     {
    //         mContext->OMSetBlendState(mAlphaBlendState.Get(), nullptr, 0xFFFFFFFFu);
    //     }
    //     else
    //     {
    //         mContext->OMSetBlendState(mNoAlphaBlendState.Get(), nullptr, 0xFFFFFFFFu);
    //     }
    // }

    const auto instance_count = 1u;
    const auto start_index = 0u;
    const auto start_vertex = 0u;
    const auto start_instance = 0u;
    command_list_->DrawIndexedInstanced(state.mesh_->GetTriangleCount(), instance_count, start_index, start_vertex, start_instance);
}

// void D3DRenderer::UpdateBuffer(wrl::ComPtr<ID3D11Buffer> buffer, void* bufferData, size_t bufferSize)
// {
//     D3D11_MAPPED_SUBRESOURCE mappedResource;
//     mContext->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
//     std::memcpy(mappedResource.pData, bufferData, bufferSize);
//     mContext->Unmap(buffer.Get(), 0);
// }

// void D3DRenderer::CreateConstantBuffer(ID3D11Buffer** outBuffer, void* bufferPtr, size_t bufferSize)
// {
//     D3D11_BUFFER_DESC cbd;
//     cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//     cbd.Usage = D3D11_USAGE_DYNAMIC;
//     cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//     cbd.MiscFlags = 0u;
//     cbd.ByteWidth = bufferSize;
//     cbd.StructureByteStride = 0u;
//     D3D11_SUBRESOURCE_DATA csd = {};
//     csd.pSysMem = bufferPtr;
//     d3dAssert(mDevice->CreateBuffer(&cbd, &csd, outBuffer));
// }


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