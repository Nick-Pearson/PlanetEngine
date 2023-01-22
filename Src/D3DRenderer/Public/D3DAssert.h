#pragma once

#include <d3d11.h>
#include <comdef.h>

#include "PlanetLogging.h"

#define DX_DEBUG 1

#define d3dAssert(E) { \
    HRESULT r = (E); \
    if (FAILED(r)) { \
        _com_error err(r); \
        P_FATAL("!!! D3D ASSERT FAILED !!! \n\tHRESULT={:#04X}  {}\n\t{}", static_cast<int64_t>(r), err.ErrorMessage(), #E); \
    } \
    }


#if defined(DX_DEBUG)
    #define SET_NAME(obj, name) obj->SetName(L##name);
    #define SET_NAME_F(obj, name, ...) \
        { \
            wchar_t ___buff[256]; \
            swprintf(___buff, 256, L##name, ##__VA_ARGS__); \
            obj->SetName(___buff); \
        }
#else
    #define SET_NAME(obj, name)
    #define SET_NAME_F(obj, name, ...)
#endif