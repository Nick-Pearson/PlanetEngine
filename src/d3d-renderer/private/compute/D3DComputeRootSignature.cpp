#include "D3DComputeRootSignature.h"

#include <DirectXMath.h>

#include "D3DAssert.h"
#include "D3DPipelineState.h"
#include "D3DConstants.h"

D3DComputeRootSignature::D3DComputeRootSignature(const ComputeShader* compute_shader, ID3D12Device2* device) :
    compute_shader_(compute_shader)
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

    CD3DX12_ROOT_PARAMETER1 params[2];
    uint32_t num_params = 2;

    CD3DX12_DESCRIPTOR_RANGE1 desc_range{D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_UAV,
        static_cast<uint32_t>(compute_shader->GetNumTextureOutputs()),
        0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE};
    params[0].InitAsDescriptorTable(1U, &desc_range);

    CD3DX12_DESCRIPTOR_RANGE1 desc_range2{D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
        static_cast<uint32_t>(compute_shader->GetNumDataInputs()),
        0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE};
    params[1].InitAsDescriptorTable(1U, &desc_range2);

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC desc;
    desc.Init_1_1(num_params, params);

    // Serialize the root signature.
    ID3DBlob* blob;
    d3dAssert(D3DX12SerializeVersionedRootSignature(&desc, feature_data.HighestVersion, &blob, nullptr));
    // Create the root signature.
    d3dAssert(device->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&root_signature_)));
}

D3DComputeRootSignature::~D3DComputeRootSignature()
{
    root_signature_->Release();
    root_signature_ = nullptr;
}

void D3DComputeRootSignature::Bind(ID3D12GraphicsCommandList* command_list) const
{
    command_list->SetComputeRootSignature(root_signature_);
}
