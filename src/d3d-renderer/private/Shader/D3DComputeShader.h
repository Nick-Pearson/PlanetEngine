#pragma once

#include <d3d12.h>
#include <vector>

#include "Compute/NumThreads.h"

class D3DComputeShader
{
 public:
    explicit D3DComputeShader(ID3DBlob* blob);
    ~D3DComputeShader();

    inline ID3DBlob* GetBlob() const { return blob_; }

 private:
    ID3DBlob* blob_;
};