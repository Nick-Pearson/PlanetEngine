#pragma once

#include <d3d12.h>

class MeshResource
{
 public:
    virtual const D3D12_VERTEX_BUFFER_VIEW* GetVertexBuffer() const = 0;
    virtual const D3D12_INDEX_BUFFER_VIEW* GetTriangleBuffer() const = 0;
    virtual size_t GetTriangleCount() const = 0;
    virtual bool IsLoaded() const = 0;
};