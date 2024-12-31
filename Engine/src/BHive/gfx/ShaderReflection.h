#pragma once

#include "core/Core.h"

namespace BHive
{
	struct BHIVE Uniform
	{
		int32_t size;
		int32_t type;
		int32_t loc;
	};

	struct BHIVE Sampler2D
	{
		int32_t binding;
	};

	struct BHIVE UniformBufferData
	{
		int32_t binding;
		int32_t size;
	};

	struct BHIVE ShaderRelfectionData
	{
		std::unordered_map<const char*, Uniform> uniforms;
		std::unordered_map<const char*, Sampler2D> samplers;
		std::unordered_map<const char*, UniformBufferData> uniformBuffers;
	};
}