#pragma once

#include <d3d12.h>

#include "Shader/D3DVertexShader.h"
#include "Shader/D3DPixelShader.h"

class D3DPipelineState
{
 public:
    D3DPipelineState(ID3D12Device2* device,
                  ID3D12RootSignature* root_signature,
                  const D3DVertexShader* vertex_shader,
                  const D3DPixelShader* pixel_shader);
    ~D3DPipelineState();

    void Bind(ID3D12GraphicsCommandList* command_list);
    void Compile();

 private:
    ID3D12Device2* const device_;
    ID3D12RootSignature* const root_signature_;
    const D3DVertexShader* const vertex_shader_;
    const D3DPixelShader* const pixel_shader_;

    ID3D12PipelineState* pipeline_state_ = nullptr;
};