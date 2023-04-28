#pragma once

#include <d3d12.h>
#include "d3dx12.h"

#include "Shader/PixelShader.h"
#include "Shader/D3DPixelShader.h"

class D3DRootSignature
{
 public:
    explicit D3DRootSignature(const PixelShader* pixel_shader, ID3D12Device2* device);
    virtual ~D3DRootSignature();

    void Bind(ID3D12GraphicsCommandList* command_list) const;

    inline ID3D12RootSignature* GetRootSignature() const { return root_signature_; }

 private:
    ID3D12RootSignature* root_signature_;

    const PixelShader* const pixel_shader_;
};
