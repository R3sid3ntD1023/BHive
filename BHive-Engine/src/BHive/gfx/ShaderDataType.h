#pragma once

#include <stdint.h>

namespace BHive
{
	enum class EShaderDataType : uint8_t
	{
		Float,
		Float2,
		Float3,
		Float4,
		Int,
		Int2,
		Int3,
		Int4,
		Bool,
		Mat3,
		Mat4
	};

}