#pragma once

#include <d3d12.h>
#include "d3dx12.h"

#include "compute/ComputeShader.h"

class D3DComputeRootSignature
{
 public:
    explicit D3DComputeRootSignature(const ComputeShader* compute_shader, ID3D12Device2* device);
    virtual ~D3DComputeRootSignature();

    void Bind(ID3D12GraphicsCommandList* command_list) const;

    inline ID3D12RootSignature* GetRootSignature() const { return root_signature_; }

 private:
    ID3D12RootSignature* root_signature_;

    const ComputeShader* const compute_shader_;
};
