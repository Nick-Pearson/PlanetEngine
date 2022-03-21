#include "D3DRootSignature.h"

#include <DirectXMath.h>

#include "D3DAssert.h"
#include "D3DPipelineState.h"
#include "D3DConstants.h"

D3DRootSignature::D3DRootSignature(const D3DVertexShader* vertex_shader, ID3D12Device2* device) :
    vertex_shader_(vertex_shader)
{
    ied_[0] = {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0};
    ied_[1] = {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0};
    ied_[2] = {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0};

    D3D12_FEATURE_DATA_ROOT_SIGNATURE feature_data = {};
    feature_data.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
    if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &feature_data, sizeof(feature_data))))
    {
        feature_data.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }

    D3D12_ROOT_SIGNATURE_FLAGS flags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

    CD3DX12_ROOT_PARAMETER1 params[2];
    params[0].InitAsConstants(D3DSlowVSConstants::size_32_bit_, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
    params[1].InitAsConstants(D3DFastVSConstants::size_32_bit_, 1, 0, D3D12_SHADER_VISIBILITY_VERTEX);

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC desc;
    desc.Init_1_1(_countof(params), params, 0, nullptr, flags);

    // Serialize the root signature.
    ID3DBlob* blob;
    d3dAssert(D3DX12SerializeVersionedRootSignature(&desc, feature_data.HighestVersion, &blob, nullptr));
    // Create the root signature.
    d3dAssert(device->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&root_signature_)));
}

D3DRootSignature::~D3DRootSignature()
{
    root_signature_->Release();
    root_signature_ = nullptr;
}

void D3DRootSignature::Bind(ID3D12GraphicsCommandList* command_list) const
{
    command_list->SetGraphicsRootSignature(root_signature_);
}

D3DPipelineState* D3DRootSignature::NewPipelineState(const D3DPixelShader* pixel_shader) const
{
    return new D3DPipelineState{
        this,
        vertex_shader_,
        pixel_shader
    };
}