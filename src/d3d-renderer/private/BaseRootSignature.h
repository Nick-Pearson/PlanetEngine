#pragma once

#include <d3d12.h>
#include "d3dx12.h"

#include "D3DAssert.h"

class BaseRootSignature
{
 public:
    explicit BaseRootSignature(ID3D12Device2* device);
    virtual ~BaseRootSignature();

    void Bind(ID3D12GraphicsCommandList* command_list) const;

 private:
    ID3D12RootSignature* root_signature_;
};