#include "D3DComputeShader.h"

D3DComputeShader::D3DComputeShader(ID3DBlob* blob, ID3D11ComputeShader* handle, const NumThreads& num_threads) :
    blob_(blob), handle_(handle), num_threads_(num_threads)
{
}

D3DComputeShader::~D3DComputeShader()
{
    blob_->Release();
    handle_->Release();
    for (auto uav : uavs_)
        uav->Release();
    for (auto res : resources_)
        res->Release();
}

void D3DComputeShader::AddUAV(ID3D11UnorderedAccessView* uav)
{
    if (uav)
        uavs_.push_back(uav);
}

void D3DComputeShader::AddResource(ID3D11ShaderResourceView* res)
{
    if (res)
        resources_.push_back(res);
}

void D3DComputeShader::Invoke(ID3D11DeviceContext* context)
{
    context->CSSetShader(handle_, nullptr, 0u);
    if (!uavs_.empty())
    {
        context->CSSetUnorderedAccessViews(0u, uavs_.size(), uavs_.data(), nullptr);
    }

    if (!resources_.empty())
    {
        context->CSSetShaderResources(0u, resources_.size(), resources_.data());
    }

    context->Dispatch(num_threads_.x_, num_threads_.y_, num_threads_.z_);
    // TODO: fix for multiple inputs
    ID3D11UnorderedAccessView* null_uav = nullptr;
    context->CSSetUnorderedAccessViews(0u, 1u, &null_uav, nullptr);
    ID3D11ShaderResourceView* null_res = nullptr;
    context->CSSetShaderResources(0u, 1u, &null_res);
}
