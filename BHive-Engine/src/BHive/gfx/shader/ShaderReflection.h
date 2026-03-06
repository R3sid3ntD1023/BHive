#pragma once

#include "core/Core.h"
#include "gfx/Enumerations.h"

namespace BHive
{
	struct FUniform
	{
		int32_t Type;
		int32_t Size;
		int32_t Offset;
		int32_t Location;
		EShaderStage Stages;

		template <typename A>
		void Serialize(A &ar)
		{
			ar(Type, Size, Offset, Location, Stages);
		}
	};

	struct FSampler
	{
		int32_t Set;
		int32_t Binding;
		EShaderStage Stages;
		uint32_t ArraySize;
		EResourceType Type;

		template <typename A>
		void Serialize(A &ar)
		{
			ar(Set, Binding, Stages, ArraySize, Type);
		}
	};

	struct FUniformBuffer
	{
		int32_t Set;
		int32_t Binding;
		int32_t Size;
		EShaderStage Stages;
		std::unordered_map<std::string, FUniform> Members;
		static constexpr EResourceType Type = EResourceType::UniformBuffer;

		template <typename A>
		void Serialize(A &ar)
		{
			ar(Set, Binding, Size, Stages, Members);
		}
	};

	struct FPushConstantsRange
	{
		
		int32_t Size;
		int32_t Offset;
		EShaderStage Stages;
		std::unordered_map<std::string, FUniform> Members;

		template <typename A>
		void Serialize(A &ar)
		{
			ar(Size, Offset, Stages, Members);
		}
	};

	struct FStorageBuffer
	{
		int32_t Set;
		int32_t Binding;
		int32_t Size;
		EShaderStage Stages;

		static constexpr EResourceType Type = EResourceType::StorageBuffer;

		template <typename A>
		void Serialize(A &ar)
		{
			ar(Set, Binding, Size, Stages);
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

	struct FReflectedResource
	{
		enum class Kind
		{
			None,
			UBO,
			SSBO,
			Sampler,
			PushConstant,
			PlainUniform
		};

		Kind kind = Kind::None;
		uint32_t set = 0;
		uint32_t binding = 0;
		uint32_t offset = 0;
		uint32_t size = 0;
		uint32_t location = 0;
	};


	class FShaderReflectionLookUp
	{
	public:
		FShaderReflectionLookUp() = default;
		FShaderReflectionLookUp(const FShaderReflection &merged);

		const FReflectedResource *FindByName(const std::string &name) const;

		const FReflectedResource *FindBySetBinding(uint32_t set, uint32_t binding) const;

		const std::vector<FReflectedResource> &GetSetBindings(uint32_t set) const;

		uint32_t GetMaxSet() const { return mMaxSet; }

	private:
		void Build(const FShaderReflection &merged);

	private:
		std::unordered_map<std::string, FReflectedResource> mByName;
		std::unordered_map < uint32_t, std::unordered_map<uint32_t, FReflectedResource>> mBySetBinding;
		std::unordered_map<uint32_t, std::vector<FReflectedResource>> mSets;
		uint32_t mMaxSet = 0;
	};
} // namespace BHive