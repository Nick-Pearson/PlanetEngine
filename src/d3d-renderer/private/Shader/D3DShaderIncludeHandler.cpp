#include "D3DShaderIncludeHandler.h"

#include <codecvt>
#include <locale>

#include "PlanetLogging.h"

namespace
{
    std::wstring dirpath(std::wstring file)
    {
        auto idx = file.rfind('/');
        if (idx == -1)
        {
            return file;
        }
        return file.substr(0, idx+1);
    }
}  // namespace


D3DShaderIncludeHandler::D3DShaderIncludeHandler(std::wstring main_filepath) :
    main_filepath_(main_filepath)
{
}

HRESULT D3DShaderIncludeHandler::Open(D3D_INCLUDE_TYPE IncludeType,
            const char* pFileName, const void* pParentData,
            const void** ppData, uint32_t* pBytes)
{
    std::wstring wide_file_name = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(pFileName);
    P_LOG("processing include {}", pFileName);

    if (IncludeType == D3D_INCLUDE_LOCAL)
    {
        auto path = dirpath(main_filepath_);
        path.append(wide_file_name);

        constexpr size_t buffer_size = 1024;
        wchar_t* fullpath = new wchar_t[buffer_size];
        GetFullPathNameW(path.c_str(), buffer_size, fullpath, nullptr);
        auto handle = CreateFileW(fullpath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (handle == INVALID_HANDLE_VALUE) return 1;

        auto file_size = GetFileSize(handle, nullptr);
        void* data = new uint8_t[file_size];
        DWORD bytes_read;
        auto result = ReadFile(handle, data, file_size, &bytes_read, nullptr);
        if (result == 0) return 1;

        *ppData = data;
        *pBytes = bytes_read;

        delete[] fullpath;
        return S_OK;
    }
    else
    {
        P_FATAL("Unsupported include type {}", IncludeType);
        return 1;
    }
}

HRESULT D3DShaderIncludeHandler::Close(const void* pData)
{
    delete[] pData;
    return S_OK;
}
