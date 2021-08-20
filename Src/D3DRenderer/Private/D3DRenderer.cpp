#include "D3DRenderer.h"

#include <wrl/client.h>
#include <D3Dcompiler.h>
#include <memory>
#include <vector>

#include "Platform/Window.h"
#include "Shader/D3DShader.h"
#include "Shader/D3DShaderLoader.h"
#include "Texture/D3DTexture.h"
#include "GPUResourceManager.h"
#include "Mesh/Mesh.h"
#include "World/CameraComponent.h"
#include "Math/Transform.h"

D3DRenderer::D3DRenderer(
    wrl::ComPtr<ID3D11Device> device,
    wrl::ComPtr<IDXGISwapChain> swapChain,
    wrl::ComPtr<ID3D11DeviceContext> context) :
    mDevice(device), mSwapChain(swapChain), mContext(context)
{
    // apply depth buffer
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    d3dAssert(mDevice->CreateDepthStencilState(&dsDesc, use_depth_stencil_state_.GetAddressOf()));
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
    d3dAssert(mDevice->CreateDepthStencilState(&dsDesc, no_depth_stencil_state_.GetAddressOf()));

    // bind depth state
    mContext->OMSetDepthStencilState(use_depth_stencil_state_.Get(), 1u);

    // Compile Shaders
    auto shader_loader = new D3DShaderLoader{ device };
    vertexShader = shader_loader->LoadVertex("VertexShader.hlsl");
    delete shader_loader;

    // bind vertex layout
    mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    vertexShader->Use(mContext.Get());

    CreateConstantBuffer(&mSlowConstantBuffer, &mSlowConstantBufferData, sizeof(mSlowConstantBufferData));
    CreateConstantBuffer(&mFastConstantBuffer, &mFastConstantBufferData, sizeof(mFastConstantBufferData));

    ID3D11Buffer* VertBuffers[2] = { mSlowConstantBuffer.Get(), mFastConstantBuffer.Get() };
    mContext->VSSetConstantBuffers(0u, 2u, VertBuffers);

    CreateConstantBuffer(&mWorldPixelBuffer, &mWorldPixelBufferData, sizeof(mWorldPixelBufferData));

    ID3D11Buffer* PixBuffers[1] = { mWorldPixelBuffer.Get() };
    mContext->PSSetConstantBuffers(0u, 1u, PixBuffers);

    {
        D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC{ CD3D11_DEFAULT{} };
        auto& brt = blendDesc.RenderTarget[0];
        brt.BlendEnable = TRUE;
        brt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
        brt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        brt.BlendOp = D3D11_BLEND_OP_ADD;
        brt.SrcBlendAlpha = D3D11_BLEND_ZERO;
        brt.DestBlendAlpha = D3D11_BLEND_ZERO;
        brt.BlendOpAlpha = D3D11_BLEND_OP_ADD;
        brt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        d3dAssert(mDevice->CreateBlendState(&blendDesc, &mAlphaBlendState));
    }
    {
        D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC{ CD3D11_DEFAULT{} };
        auto& brt = blendDesc.RenderTarget[0];
        brt.BlendEnable = FALSE;
        brt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        d3dAssert(mDevice->CreateBlendState(&blendDesc, &mNoAlphaBlendState));
    }
    mContext->OMSetBlendState(mNoAlphaBlendState.Get(), nullptr, 0xFFFFFFFFu);
}

D3DRenderer::~D3DRenderer()
{
}

void D3DRenderer::BindRenderTarget(const RenderTarget& target)
{
    if (render_target_view_)
        render_target_view_->Release();
    if (depth_stencil_view_)
        depth_stencil_view_->Release();

    D3D11_VIEWPORT vp;
    vp.Width = static_cast<float>(target.GetWidth());
    vp.Height = static_cast<float>(target.GetHeight());
    vp.MinDepth = 0;
    vp.MaxDepth = 1;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    mContext->RSSetViewports(1u, &vp);

    aspect_ratio_ = vp.Height / vp.Width;

    render_target_view_ = target.GetRenderTarget();
    render_target_view_->AddRef();
    depth_stencil_view_ = target.GetDepthStencil();
    depth_stencil_view_->AddRef();
    mContext->OMSetRenderTargets(1u, &render_target_view_, depth_stencil_view_);
    P_LOG("Set render size to {}x{}", target.GetWidth(), target.GetHeight());
}

void D3DRenderer::Render(const CameraComponent& camera)
{
    PreRender(camera);

    // sort render states
    // use array sort for better cache performance in the sort and the subsequent loop
    std::vector<RenderState> sortedStates;
    renderStates.SortToArray([&](const RenderState& a, const RenderState& b) {
        if (!a.UseDepthBuffer && b.UseDepthBuffer) return true;
        return false;
    }, &sortedStates);
    // draw each state
    for (const RenderState& state : sortedStates)
    {
        Draw(camera, state);
    }

    PostRender();
}

void D3DRenderer::PreRender(const CameraComponent& camera)
{
    const float colour[4] = { 0.f, 0.f, 0.f, 1.0f };
    mContext->ClearRenderTargetView(render_target_view_, colour);
    mContext->ClearDepthStencilView(depth_stencil_view_, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);

    // setup projection matrix
    // TODO: Maybe not do this every frame?
    // TODO: Collect stats on how often these buffers are updated
    mSlowConstantBufferData.view = DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveLH(1.0f, aspect_ratio_, camera.NearClip, camera.FarClip));
    Transform cameraTransform = camera.GetWorldTransform();
    cameraTransform.location = Vector{};
    cameraTransform.scale = Vector{1.0f, 1.0f, 1.0f};
    DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(cameraTransform.GetMatrix());
    mSlowConstantBufferData.world = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, cameraTransform.GetMatrix()));

    UpdateBuffer(mSlowConstantBuffer, &mSlowConstantBufferData, sizeof(mSlowConstantBufferData));
    currentRenderState.UseWorldMatrix = false;
}

void D3DRenderer::PostRender()
{
    for (unsigned int i = 0; i < used_texture_slots_; ++i)
    {
        ID3D11ShaderResourceView* null_srv = nullptr;
        mContext->PSSetShaderResources(i, 1u, &null_srv);
    }

    currentRenderState = RenderState{};
    used_texture_slots_ = 0;
}

RenderState* D3DRenderer::AddRenderState(const RenderState& state)
{
    return renderStates.Add(state);
}

void D3DRenderer::RemoveRenderState(const RenderState* state)
{
    renderStates.Remove(state);
}

void D3DRenderer::UpdateWorldBuffer(const WorldBufferData& data)
{
    mWorldPixelBufferData = data;
    UpdateBuffer(mWorldPixelBuffer, &mWorldPixelBufferData, sizeof(mWorldPixelBufferData));
}

void D3DRenderer::Draw(const CameraComponent& camera, const RenderState& state)
{
    if (!state.IsValid()) return;

    // apply the render state
    if (currentRenderState.UseDepthBuffer != state.UseDepthBuffer)
    {
        if (state.UseDepthBuffer)
        {
            mContext->OMSetDepthStencilState(use_depth_stencil_state_.Get(), 1u);
        }
        else
        {
            mContext->OMSetDepthStencilState(no_depth_stencil_state_.Get(), 1u);
        }
    }

    if (currentRenderState.UseWorldMatrix != state.UseWorldMatrix)
    {
        Transform cameraTransform = camera.GetWorldTransform();
        if (!state.UseWorldMatrix)
        {
            cameraTransform.location = Vector{};
            cameraTransform.scale = Vector{1.0f, 1.0f, 1.0f};
        }
        DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(cameraTransform.GetMatrix());
        mSlowConstantBufferData.world = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, cameraTransform.GetMatrix()));

        UpdateBuffer(mSlowConstantBuffer, &mSlowConstantBufferData, sizeof(mSlowConstantBufferData));
    }

    DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(state.model.GetMatrix());
    mFastConstantBufferData.model = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, state.model.GetMatrix()));
    UpdateBuffer(mFastConstantBuffer, &mFastConstantBufferData, sizeof(mFastConstantBufferData));

    if (currentRenderState.mesh != state.mesh)
    {
        const unsigned int stride = sizeof(Vertex);
        const unsigned int offset = 0u;
        mContext->IASetVertexBuffers(0u, 1u, state.mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
        mContext->IASetIndexBuffer(state.mesh->triangleBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
    }

    if (currentRenderState.material != state.material)
    {
        state.material->shader->Use(mContext.Get());
        unsigned int num_textures = state.material->textures.size();
        for (unsigned int i = 0; i < num_textures; ++i)
        {
            state.material->textures[i]->Use(mContext.Get(), i);
        }
        used_texture_slots_ = num_textures > used_texture_slots_ ? num_textures : used_texture_slots_;

        if (state.material->alpha)
        {
            mContext->OMSetBlendState(mAlphaBlendState.Get(), nullptr, 0xFFFFFFFFu);
        }
        else
        {
            mContext->OMSetBlendState(mNoAlphaBlendState.Get(), nullptr, 0xFFFFFFFFu);
        }
    }

    currentRenderState = state;

    mContext->DrawIndexed(state.mesh->numTriangles, 0u, 0u);
}

void D3DRenderer::UpdateBuffer(wrl::ComPtr<ID3D11Buffer> buffer, void* bufferData, size_t bufferSize)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    mContext->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    std::memcpy(mappedResource.pData, bufferData, bufferSize);
    mContext->Unmap(buffer.Get(), 0);
}

void D3DRenderer::CreateConstantBuffer(ID3D11Buffer** outBuffer, void* bufferPtr, size_t bufferSize)
{
    D3D11_BUFFER_DESC cbd;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbd.MiscFlags = 0u;
    cbd.ByteWidth = bufferSize;
    cbd.StructureByteStride = 0u;
    D3D11_SUBRESOURCE_DATA csd = {};
    csd.pSysMem = bufferPtr;
    d3dAssert(mDevice->CreateBuffer(&cbd, &csd, outBuffer));
}
