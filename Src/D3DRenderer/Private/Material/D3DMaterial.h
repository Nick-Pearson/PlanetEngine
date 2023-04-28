#pragma once

#include <d3d12.h>
#include <vector>

#include "../Descriptor/D3DDescriptorTable.h"
#include "../Texture/D3DTexture.h"

class D3DMaterial
{
 public:
    D3DMaterial(D3DDescriptorTable* descriptor_table, const std::vector<const D3DTexture*>& textures);
    ~D3DMaterial();

    void Bind(ID3D12GraphicsCommandList* command_list);
    inline bool IsLoaded() const { return loaded_; }

    void OnLoadingComplete();

 private:
    bool loaded_ = false;
    bool first_bind_ = true;

    D3DDescriptorTable* const descriptor_table_;
    std::vector<const D3DTexture*> textures_;
};