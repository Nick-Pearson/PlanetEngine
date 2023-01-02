#pragma once

#include <d3d12.h>
#include <vector>

#include "D3DRootSignature.h"
#include "D3DPipelineState.h"
#include "../Descriptor/D3DDescriptorTable.h"
#include "../Texture/D3DTexture.h"

class D3DMaterial
{
 public:
    D3DMaterial(D3DRootSignature* root_signature,
         D3DPipelineState* pipeline_state,
         D3DDescriptorTable* descriptor_table,
         const std::vector<const D3DTexture*>& textures);
    ~D3DMaterial();

    inline D3DRootSignature* GetRootSignature() const { return root_signature_; }

    void Bind(ID3D12GraphicsCommandList* command_list);
    inline bool IsLoaded() const { return loaded_; }

    void OnLoadingComplete();

 private:
    bool loaded_ = false;
    bool first_bind_ = true;

    D3DRootSignature* const root_signature_;
    D3DPipelineState* const pipeline_state_;
    D3DDescriptorTable* const descriptor_table_;
    std::vector<const D3DTexture*> textures_;
};