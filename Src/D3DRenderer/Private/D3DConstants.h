#pragma once

#include <cstdint>

namespace D3DSlowVSConstants
{

struct Data
{
    DirectX::XMMATRIX world_;
    DirectX::XMMATRIX view_;
};

constexpr unsigned int size_32_bit_ = sizeof(Data) / sizeof(uint32_t);
}  // namespace D3DSlowVSConstants

namespace D3DFastVSConstants
{

struct Data
{
    DirectX::XMMATRIX model_;
};

constexpr unsigned int size_32_bit_ = sizeof(Data) / sizeof(uint32_t);
}  // namespace D3DFastVSConstants