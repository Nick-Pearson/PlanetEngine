#include "D3DRootSignature.h"

#include <DirectXMath.h>

#include "D3DAssert.h"
#include "D3DPipelineState.h"
#include "D3DConstants.h"

D3DRootSignature::D3DRootSignature(const PixelShader* pixel_shader, ID3D12Device2* device) :
    pixel_shader_(pixel_shader)
{
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
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

    CD3DX12_ROOT_PARAMETER1 params[4];
    uint32_t num_params = 3;
    params[0].InitAsConstants(D3DSlowVSConstants::size_32_bit_, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
    params[1].InitAsConstants(D3DFastVSConstants::size_32_bit_, 1, 0, D3D12_SHADER_VISIBILITY_VERTEX);
    params[2].InitAsConstants(D3DWorldPSConstants::size_32_bit_, 0, 0, D3D12_SHADER_VISIBILITY_PIXEL);

    auto num_inputs = pixel_shader->GetNumInputs();
    if (num_inputs > 0)
    {
        num_params++;
        P_ASSERT(num_inputs == 1, "Must have only 1 input");

        CD3DX12_DESCRIPTOR_RANGE1 desc_range{D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0};
        params[3].InitAsDescriptorTable(1U, &desc_range, D3D12_SHADER_VISIBILITY_PIXEL);
    }

    // create a static sampler
    D3D12_STATIC_SAMPLER_DESC sampler = {};
    sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
    sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    sampler.MipLODBias = 0;
    sampler.MaxAnisotropy = 0;
    sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
    sampler.MinLOD = 0.0f;
    sampler.MaxLOD = D3D12_FLOAT32_MAX;
    sampler.ShaderRegister = 0;
    sampler.RegisterSpace = 0;
    sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC desc;
    desc.Init_1_1(num_params, params, num_inputs, &sampler, flags);

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
