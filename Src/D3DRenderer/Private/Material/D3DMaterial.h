#pragma once

#include <d3d12.h>
#include <memory>
#include <vector>

#include "D3DRootSignature.h"
#include "D3DPipelineState.h"
#include "../Descriptor/D3DDescriptorTable.h"

class D3DMaterial
{
 public:
    D3DMaterial(D3DRootSignature* root_signature, D3DPipelineState* pipeline_state, D3DDescriptorTable* descriptor_table);
    ~D3DMaterial();

    inline D3DRootSignature* GetRootSignature() const { return root_signature_; }

    void Bind(ID3D12GraphicsCommandList* command_list);
    inline bool IsLoaded() const { return loaded_; }

    void OnLoadingComplete();

 private:
    bool loaded_ = false;

    D3DRootSignature* const root_signature_;
    D3DPipelineState* const pipeline_state_;
    D3DDescriptorTable* const descriptor_table_;
};