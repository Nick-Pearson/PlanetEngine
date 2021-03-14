#pragma once

#include <d3d11.h>

#include "PlanetLogging.h"

#define d3dAssert(E) { HRESULT r = (E); if (r != S_OK) { P_ERROR("!! D3D ASSERT FAILED   {}", #E); } }
