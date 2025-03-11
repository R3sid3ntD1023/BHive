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
		std::unordered_map<std::string, FUniform> Uniforms;
	};

	struct FStorageBuffer
	{
		int32_t Binding{};
		int32_t Size{};
	};

	struct FShaderReflectionData
	{
		FShaderReflectionData() = default;
		FShaderReflectionData(const FShaderReflectionData &) = default;
		FShaderReflectionData(uint32_t program);

		std::unordered_map<std::string, FUniform> Uniforms;
		std::unordered_map<std::string, FSampler2D> Samplers;
		std::unordered_map<std::string, FUniformBufferData> UniformBuffers;
		std::unordered_map<std::string, FStorageBuffer> StorageBuffers;

	private:
		void GetUniforms(uint32_t program);
		void GetUniformBuffers(uint32_t program);
		void GetStorageBuffers(uint32_t program);
		void GetSamplers(uint32_t program);
	};
} // namespace BHive