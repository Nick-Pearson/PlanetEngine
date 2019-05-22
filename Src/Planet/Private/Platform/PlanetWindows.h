#pragma once

#ifdef WINVER
#undef WINVER
#endif

#define WIN32_LEAN_AND_MEAN
#define WINVER 0x0500

#include <Windows.h>