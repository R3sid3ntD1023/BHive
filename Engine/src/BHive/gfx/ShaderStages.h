#pragma once

#include <stdint.h>

namespace BHive
{
	enum EShaderStage : uint8_t
	{
		ShaderStage_None = 0,
		ShaderStage_Vertex,
		ShaderStage_Fragment,
		ShaderStage_Compute,
		ShaderStage_Geometry,
	};
} // namespace BHive