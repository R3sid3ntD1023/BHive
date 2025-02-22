#pragma once

#include "ShaderInstance.h"
#include "gfx/Shader.h"

namespace BHive
{
	template <typename T>
	inline void ShaderInstance::SetParameter(const char *parameterName, const T &v)
	{
		if (!mShaderUniforms.contains(parameterName))
		{
			LOG_WARN("No uniform found with name {}", parameterName);
			return;
		}

		auto &uniform = mShaderUniforms.at(parameterName);
		uint8_t *ptr = mInstanceData + uniform.Offset;
		memcpy(ptr, &v, uniform.Size);
	}
} // namespace BHive