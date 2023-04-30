#pragma once

#include <cstdint>

namespace D3DSlowVSConstants
{

struct Data
{
    DirectX::XMMATRIX world_;
    DirectX::XMMATRIX view_;
};

static_assert(sizeof(Data) % sizeof(uint32_t) == 0, "Must be multiple of 32");
constexpr unsigned int size_32_bit_ = sizeof(Data) / sizeof(uint32_t);
}  // namespace D3DSlowVSConstants

namespace D3DFastConstants
{

struct Data
{
    DirectX::XMMATRIX model_;
    DirectX::XMFLOAT3 sun_dir_;
    float sun_sky_strength_;
    DirectX::XMFLOAT3 sun_col_;
};

static_assert(sizeof(Data) % sizeof(uint32_t) == 0, "Must be multiple of 32");
constexpr unsigned int size_32_bit_ = sizeof(Data) / sizeof(uint32_t);
}  // namespace D3DFastConstants
