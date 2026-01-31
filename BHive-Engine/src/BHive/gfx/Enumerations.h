#pragma once

#include "core/Core.h"

namespace BHive
{
	enum EDrawMode
	{
		Lines = 0x0001,
		Triangles = 0x0004
	};

	enum ECullMode : uint8_t
	{
		Cull_None = 0,
		Cull_Front = 1,
		Cull_Back = 2
	};

	enum ClearBitMask : int
	{
		Buffer_Depth = 0x00000100,
		Buffer_Stencil = 0x00000400,
		Buffer_Color = 0x00004000,
	};


}