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

	struct FSampler
	{
		int32_t Binding{};
		EShaderStage Stages{};
		uint32_t ArraySize{};

		template <typename A>
		void Serialize(A &ar)
		{
			ar(Binding, Stages, ArraySize);
		}
	};

	struct FUniformBuffer
	{
		int32_t Binding{};
		int32_t Size{};
		EShaderStage Stages{};
		std::unordered_map<std::string, FUniform> Members;

		template <typename A>
		void Serialize(A &ar)
		{
			ar(Binding, Size, Stages, Members);
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
		int32_t Binding{};
		int32_t Size{};
		EShaderStage Stages{};

		template <typename A>
		void Serialize(A &ar)
		{
			ar(Binding, Size, Stages);
		}
	};

	struct FSetReflection
	{
		std::unordered_map<std::string, FUniformBuffer> UniformBuffers;
		std::unordered_map<std::string, FStorageBuffer> StorageBuffers;
		std::unordered_map<std::string, FSampler> Samplers;

		/*void Find();

		bool Contains();*/

		template <typename A>
		void Serialize(A &ar)
		{
			ar(UniformBuffers, StorageBuffers, Samplers);
		}
	};

	struct FShaderReflection
	{
		void Reflect(EShaderStage stage, const std::vector<uint32_t> &source);

		std::string to_string() const;

		static FShaderReflection Merge(const std::unordered_map<EShaderStage, FShaderReflection>& refl);

		std::unordered_map<uint32_t, FSetReflection> Sets;
		std::unordered_map<std::string, FUniform> Uniforms; //unused in vulkan
		std::vector<FPushConstantsRange> PushConstants; //unused in opengl
	
		template<typename A>
		void Serialize(A& ar)
		{
			ar(Sets, Uniforms, PushConstants);
		}

	};

	using FShaderReflectionDatas = std::unordered_map<EShaderStage, FShaderReflection>;
} // namespace BHive