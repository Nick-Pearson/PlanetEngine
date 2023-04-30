#pragma once

#include <d3d12.h>
#include <string>

class D3DShaderIncludeHandler : public ID3DInclude
{
 public:
    explicit D3DShaderIncludeHandler(std::wstring main_filepath);

    HRESULT __declspec(nothrow) Open(D3D_INCLUDE_TYPE IncludeType,
            const char* pFileName, const void* pParentData,
            const void** ppData, uint32_t* pBytes) override;

    HRESULT __declspec(nothrow) Close(const void* pData) override;
 private:
    const std::wstring main_filepath_;
};