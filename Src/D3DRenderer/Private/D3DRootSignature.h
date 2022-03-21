#pragma once

#include <d3d12.h>
#include "d3dx12.h"

#include "Shader/D3DVertexShader.h"
#include "Shader/D3DPixelShader.h"

class D3DRootSignature
{
    friend class D3DPipelineState;
 public:
    explicit D3DRootSignature(const D3DVertexShader* vertex_shader, ID3D12Device2* device);
    virtual ~D3DRootSignature();

    void Bind(ID3D12GraphicsCommandList* command_list) const;

    D3DPipelineState* NewPipelineState(const D3DPixelShader* pixel_shader) const;

 private:
    D3D12_INPUT_ELEMENT_DESC ied_[3];
    ID3D12RootSignature* root_signature_;

    const D3DVertexShader* const vertex_shader_;
};
