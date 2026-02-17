#pragma once

#include "core/Core.h"
#include "gfx/Enumerations.h"

namespace BHive
{
	struct FUniform
	{
		int32_t Type{};
		int32_t Size{};
		int32_t Offset{};
		int32_t Location{};
		EShaderStage Stages{};

		template <typename A>
		void Serialize(A &ar)
		{
			ar(Type, Size, Offset, Location, Stages);
		}
	};

	struct FSampler2D
	{
		int32_t Set{};
		int32_t Binding{};
		EShaderStage Stages{};

		template <typename A>
		void Serialize(A &ar)
		{
			ar(Set, Binding, Stages);
		}
	};

	struct FUniformBufferData
	{
		int32_t Set{};
		int32_t Binding{};
		int32_t Size{};
		EShaderStage Stages{};
		std::unordered_map<std::string, FUniform> Members;

		template <typename A>
		void Serialize(A &ar)
		{
			ar(Set, Binding, Size, Stages, Members);
		}
	};

	struct FPushConstantsRange
	{
		int32_t Size{};
		int32_t Offset{};
		EShaderStage Stages{};
		std::unordered_map<std::string, FUniform> Members;

		template <typename A>
		void Serialize(A &ar)
		{
			ar(Size, Offset, Stages, Members);
		}
	};

	struct FStorageBuffer
	{
		int32_t Set{};
		int32_t Binding{};
		int32_t Size{};
		EShaderStage Stages{};

		template <typename A>
		void Serialize(A &ar)
		{
			ar(Set, Binding, Size, Stages);
		}
	};

	struct FShaderReflection
	{
		void Reflect(EShaderStage stage, const std::vector<uint32_t> &source);

		std::string to_string() const;

		static FShaderReflection Merge(const std::unordered_map<EShaderStage, FShaderReflection>& refl);

		std::unordered_map<std::string, FUniform> Uniforms;
		std::unordered_map<std::string, FSampler2D> Samplers;
		std::unordered_map<std::string, FUniformBufferData> UniformBuffers;
		std::vector<FPushConstantsRange> PushConstants; //unused in opengl
		std::unordered_map<std::string, FStorageBuffer> StorageBuffers;

		template<typename A>
		void Serialize(A& ar)
		{
			ar(Uniforms, Samplers, UniformBuffers, PushConstants, StorageBuffers);
		}
	};

	using FShaderReflectionDatas = std::unordered_map<EShaderStage, FShaderReflection>;
} // namespace BHive