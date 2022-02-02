#pragma once

#include <d3d11.h>
#include <comdef.h>

#include "PlanetLogging.h"

#define d3dAssert(E) { \
    HRESULT r = (E); \
    if (FAILED(r)) { \
        _com_error err(r); \
        P_FATAL("!!! D3D ASSERT FAILED !!! \n\tHRESULT={:#04X}  {}\n\t{}", static_cast<int64_t>(r), err.ErrorMessage(), #E); \
    } \
    }
