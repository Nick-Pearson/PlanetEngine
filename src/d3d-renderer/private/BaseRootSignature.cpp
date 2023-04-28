#include "BaseRootSignature.h"

BaseRootSignature::BaseRootSignature(ID3D12Device2* device)
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
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC desc;
    desc.Init_1_1(0, nullptr, 0, nullptr, flags);

    // Serialize the root signature.
    ID3DBlob* blob;
    d3dAssert(D3DX12SerializeVersionedRootSignature(&desc, feature_data.HighestVersion, &blob, nullptr));
    // Create the root signature.
    d3dAssert(device->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&root_signature_)));
}

BaseRootSignature::~BaseRootSignature()
{
    root_signature_->Release();
    root_signature_ = nullptr;
}

void BaseRootSignature::Bind(ID3D12GraphicsCommandList* command_list) const
{
    command_list->SetGraphicsRootSignature(root_signature_);
}
