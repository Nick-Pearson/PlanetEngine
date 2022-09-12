#include "D3DPipelineState.h"

#include "d3dx12.h"

#include "D3DAssert.h"

D3DPipelineState::D3DPipelineState(ID3D12Device2* device,
                                ID3D12RootSignature* root_signature,
                                const D3DVertexShader* vertex_shader,
                                const D3DPixelShader* pixel_shader) :
    device_(device), root_signature_(root_signature), vertex_shader_(vertex_shader), pixel_shader_(pixel_shader)
{
}

D3DPipelineState::~D3DPipelineState()
{
    pipeline_state_->Release();
    pipeline_state_ = nullptr;
}

struct PipelineStateStream
{
    CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
    CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
    CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
    CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER Rasterizer;
    CD3DX12_PIPELINE_STATE_STREAM_VS VS;
    CD3DX12_PIPELINE_STATE_STREAM_PS PS;
    CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL DepthStencil;
    CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
    CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
};

void D3DPipelineState::Compile()
{
    P_ASSERT(pipeline_state_ == nullptr, "Trying to compile pipeline state twice")

    D3D12_RT_FORMAT_ARRAY rtvFormats = {};
    rtvFormats.NumRenderTargets = 3;
    rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

    CD3DX12_DEPTH_STENCIL_DESC depth_stencil{};
    depth_stencil.DepthEnable = true;
    depth_stencil.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depth_stencil.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

    CD3DX12_RASTERIZER_DESC rasterizer{};
    rasterizer.FillMode = D3D12_FILL_MODE_SOLID;
    rasterizer.CullMode = D3D12_CULL_MODE_BACK;

    PipelineStateStream state;
    state.pRootSignature = root_signature_;
    state.InputLayout = vertex_shader_->GetInputLayout();
    state.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    state.Rasterizer = rasterizer;
    state.VS = CD3DX12_SHADER_BYTECODE(vertex_shader_->GetBlob());
    state.PS = CD3DX12_SHADER_BYTECODE(pixel_shader_->GetBlob());
    state.DepthStencil = depth_stencil;
    state.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    state.RTVFormats = rtvFormats;

    D3D12_PIPELINE_STATE_STREAM_DESC desc = { sizeof(PipelineStateStream), &state };
    d3dAssert(device_->CreatePipelineState(&desc, IID_PPV_ARGS(&pipeline_state_)));
}

void D3DPipelineState::Bind(ID3D12GraphicsCommandList* command_list)
{
    if (pipeline_state_ == nullptr)
    {
        Compile();
    }
    command_list->SetPipelineState(pipeline_state_);
}