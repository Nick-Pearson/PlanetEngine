#pragma once

#include <d3d12.h>
#include <memory>
#include <unordered_map>
#include <string>

#include "D3DVertexShader.h"
#include "D3DPixelShader.h"
#include "D3DComputeShader.h"

#include "Compute/ComputeShader.h"
#include "Shader/PixelShader.h"
#include "Shader/VertexShader.h"

namespace D3DShaderLoader
{
    const D3DVertexShader* LoadVertex(const VertexShader* shader);
    const D3DPixelShader* LoadPixel(const PixelShader* shader);
    const D3DComputeShader* LoadCompute(const ComputeShader* shader);
};