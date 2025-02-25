#pragma once

#include "core/Core.h"

namespace BHive
{
	struct Uniform
	{
		int32_t Type{};
		int32_t Size{};
		int32_t Offset{};
		int32_t Location{};
	};

	struct Sampler2D
	{
		int32_t Binding{};
	};

	struct UniformBufferData
	{
		int32_t Binding{};
		int32_t Size{};
	};

	struct ShaderRelfectionData
	{
		std::unordered_map<const char *, Uniform> Uniforms;
		std::unordered_map<const char *, Sampler2D> Samplers;
		std::unordered_map<const char *, UniformBufferData> UniformBuffers;
	};
} // namespace BHive