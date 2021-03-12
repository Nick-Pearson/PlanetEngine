#include "D3DRenderer.h"

#include <wrl/client.h>
#include <D3Dcompiler.h>
#include <memory>
#include <vector>

#include "Platform/Window.h"
#include "D3DShader.h"
#include "D3DTexture.h"
#include "GPUResourceManager.h"
#include "Mesh/Mesh.h"
#include "World/CameraComponent.h"
#include "Math/Transform.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxguid.lib")

D3DRenderer::D3DRenderer(HWND window, Microsoft::WRL::ComPtr <ID3D11Device> device,
    Microsoft::WRL::ComPtr <IDXGISwapChain> swapChain, Microsoft::WRL::ComPtr <ID3D11DeviceContext> context) :
    mDevice(device), mSwapChain(swapChain), mContext(context)
{
    // create depth stencil state
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    Microsoft::WRL::ComPtr <ID3D11DepthStencilState> DSState;
    d3dAssert(mDevice->CreateDepthStencilState(&dsDesc, DSState.GetAddressOf()));

    // bind depth state
    mContext->OMSetDepthStencilState(DSState.Get(), 1u);

    UpdateWindowSize(false);

    // Compile Shaders
    vertexShader = std::make_shared<D3DShader>("VertexShader.hlsl", ShaderType::Vertex, mDevice);

    Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayout;
    const D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        {"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12u, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24u, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    d3dAssert(mDevice->CreateInputLayout(
        ied, (UINT)std::size(ied),
        vertexShader->mShaderBlob->GetBufferPointer(),
        vertexShader->mShaderBlob->GetBufferSize(),
        &InputLayout));

    // bind vertex layout
    mContext->IASetInputLayout(InputLayout.Get());
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

    const auto hModDxgiDebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    // get address of DXGIGetDebugInterface in dll
    typedef HRESULT (WINAPI* DXGIGetDebugInterface)(REFIID, IDXGIInfoQueue **);
    const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface"));

    DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), mDxgiInfoQueue.GetAddressOf());
}

D3DRenderer::~D3DRenderer()
{
}

void D3DRenderer::SwapBuffers()
{
    mSwapChain->Present(1u, 0u);

    const float colour[4] = { 0.f, 0.f, 0.f, 1.0f };
    mContext->ClearRenderTargetView(mTarget.Get(), colour);
    mContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
}

void D3DRenderer::Render(const CameraComponent& camera)
{
    // setup projection matrix
    // TODO: Maybe not do this every frame?
    // TODO: Collect stats on how often these buffers are updated
    mSlowConstantBufferData.view = DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveLH(1.0f, aspectRatio, camera.NearClip, camera.FarClip));
    Transform cameraTransform = camera.GetWorldTransform();
    cameraTransform.location = Vector{};
    cameraTransform.scale = Vector{1.0f, 1.0f, 1.0f};
    DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(cameraTransform.GetMatrix());
    mSlowConstantBufferData.world = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, cameraTransform.GetMatrix()));

    UpdateBuffer(mSlowConstantBuffer, &mSlowConstantBufferData, sizeof(mSlowConstantBufferData));

    currentRenderState.UseWorldMatrix = false;

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

void D3DRenderer::UpdateWindowSize(bool resize)
{
    mContext->OMSetRenderTargets(0, 0, 0);

    // Release all outstanding references to the swap chain's buffers.
    mTarget.Reset();

    if (resize)
    {
        // Preserve the existing buffer count and format.
        // Automatically choose the width and height to match the client rect for HWNDs.
        d3dAssert(mSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0));
    }

    // Get buffer and create a render-target-view.
    ID3D11Texture2D* pBuffer;
    d3dAssert(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBuffer)));
    d3dAssert(mDevice->CreateRenderTargetView(pBuffer, NULL, mTarget.GetAddressOf()));
    pBuffer->Release();


    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    d3dAssert(mSwapChain->GetDesc(&swapChainDesc));

    // create depth stencil texture
    Microsoft::WRL::ComPtr <ID3D11Texture2D> DepthStencil = nullptr;
    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = (UINT)swapChainDesc.BufferDesc.Width;
    descDepth.Height = (UINT)swapChainDesc.BufferDesc.Height;
    descDepth.MipLevels = 1u;
    descDepth.ArraySize = 1u;
    descDepth.Format = DXGI_FORMAT_D32_FLOAT;
    descDepth.SampleDesc.Count = 1u;
    descDepth.SampleDesc.Quality = 0u;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    d3dAssert(mDevice->CreateTexture2D(&descDepth, nullptr, DepthStencil.GetAddressOf()));

    // create view of depth stencil texture
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = DXGI_FORMAT_D32_FLOAT;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0u;
    d3dAssert(mDevice->CreateDepthStencilView(DepthStencil.Get(), &descDSV, mDepthStencilView.GetAddressOf()));

    D3D11_VIEWPORT vp;
    vp.Width = static_cast<float>(swapChainDesc.BufferDesc.Width);
    vp.Height = static_cast<float>(swapChainDesc.BufferDesc.Height);
    vp.MinDepth = 0;
    vp.MaxDepth = 1;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    mContext->RSSetViewports(1u, &vp);

    aspectRatio = vp.Height / vp.Width;

    mContext->OMSetRenderTargets(1u, mTarget.GetAddressOf(), mDepthStencilView.Get());
    P_LOG("Set render size to {}x{}", swapChainDesc.BufferDesc.Width, swapChainDesc.BufferDesc.Height);
}

void D3DRenderer::Draw(const CameraComponent& camera, const RenderState& state)
{
    if (!state.IsValid()) return;

    // apply the render state
    if (currentRenderState.UseDepthBuffer != state.UseDepthBuffer)
    {
        // apply depth buffer
        D3D11_DEPTH_STENCIL_DESC dsDesc = {};
        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthWriteMask = state.UseDepthBuffer ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
        dsDesc.DepthFunc = state.UseDepthBuffer ? D3D11_COMPARISON_LESS : D3D11_COMPARISON_ALWAYS;
        Microsoft::WRL::ComPtr <ID3D11DepthStencilState> DSState;
        d3dAssert(mDevice->CreateDepthStencilState(&dsDesc, DSState.GetAddressOf()));

        // bind depth state
        mContext->OMSetDepthStencilState(DSState.Get(), 1u);
    }

    if (currentRenderState.UseWorldMatrix != state.UseWorldMatrix)
    {
        Transform cameraTransform = state.UseWorldMatrix ? camera.GetWorldTransform() : Transform();
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
        for (unsigned int i = 0; i < state.material->textures.size(); ++i)
        {
            state.material->textures[i]->Use(mContext.Get(), i);
        }

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
    d3dFlushDebugMessages();
}

void D3DRenderer::UpdateBuffer(Microsoft::WRL::ComPtr <ID3D11Buffer> buffer, void* bufferData, size_t bufferSize)
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
