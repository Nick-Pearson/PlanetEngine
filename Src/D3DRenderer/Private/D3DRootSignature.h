#pragma once

#include <d3d12.h>

class D3DRootSignature
{
 public:
    explicit D3DRootSignature(ID3DBlob* vertex_blob, ID3D12Device2* device);
    virtual ~D3DRootSignature();

    void Bind(ID3D12GraphicsCommandList* command_list);

 private:
    ID3D12RootSignature* root_signature_;
    ID3D12PipelineState* pipeline_state_;
};
