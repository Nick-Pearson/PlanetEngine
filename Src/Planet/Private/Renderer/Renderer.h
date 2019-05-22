#pragma once

#if PLATFORM_WIN
	#include "D3DRenderer.h"
	typedef D3DRenderer Renderer;
#else
#error "No renderer for platform"
#endif
