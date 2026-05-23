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

	};

	struct FSampler
	{
		int32_t Binding;
		EShaderStage Stages;
		uint32_t ArraySize;
		EResourceType Type;
	};

	struct FUniformBuffer
	{
		int32_t Binding;
		int32_t Size;
		EShaderStage Stages;
		std::unordered_map<std::string, FUniform> Members;
		static constexpr EResourceType Type = EResourceType::UniformBuffer;
	};

	struct FPushConstantsRange
	{
		
		int32_t Size;
		int32_t Offset;
		EShaderStage Stages;
		std::unordered_map<std::string, FUniform> Members;

	};

	struct FStorageBuffer
	{
		int32_t Binding;
		int32_t Size;
		EShaderStage Stages;

		static constexpr EResourceType Type = EResourceType::StorageBuffer;
	};

	struct FSetReflection
	{
		std::unordered_map<std::string, FUniformBuffer> UniformBuffers;
		std::unordered_map<std::string, FStorageBuffer> StorageBuffers;
		std::unordered_map<std::string, FSampler> Samplers;
	};

	
	struct FShaderReflection
	{
		void Reflect(EShaderStage stage, const std::vector<uint32_t> &source);

		std::string to_string() const;

		static FShaderReflection Merge(const std::unordered_map<EShaderStage, FShaderReflection>& refl);

		std::unordered_map<uint32_t, FSetReflection> Sets;
		std::unordered_map<std::string, FUniform> Uniforms; //unused in vulkan
		std::vector<FPushConstantsRange> PushConstants; //unused in opengl
	};

	

	struct FReflectedResource
	{	
		EResourceType kind = EResourceType::Invalid;
		std::string name;
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

		template <typename A>
		friend void SERIALIZE(A &, FShaderReflectionLookUp &);
	};


} // namespace BHive

#include "ShaderReflectionSerialization.inl"
