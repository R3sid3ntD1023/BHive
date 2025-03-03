#pragma once

#include "core/Core.h"

namespace BHive
{
	struct FUniform
	{
		int32_t Type{};
		int32_t Size{};
		int32_t Offset{};
		int32_t Location{};
	};

	struct FSampler2D
	{
		int32_t Binding{};
	};

	struct FUniformBufferData
	{
		int32_t Binding{};
		int32_t Size{};
	};

	struct FStorageBuffer
	{
		int32_t Binding{};
		int32_t Size{};
	};

	struct FShaderReflectionData
	{
		std::unordered_map<std::string, FUniform> Uniforms;
		std::unordered_map<std::string, FSampler2D> Samplers;
		std::unordered_map<std::string, FUniformBufferData> UniformBuffers;
		std::unordered_map<std::string, FStorageBuffer> StorageBuffers;
	};
} // namespace BHive