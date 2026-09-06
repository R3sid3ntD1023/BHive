#pragma once

#include "gfx/Enumerations.h"

namespace BHive
{
	struct BindingSetTemplate
	{
		struct Binding
		{
			uint32_t Binding;
			EResourceType Type;
			size_t Size;
			uint32_t ArraySize;
			EShaderStage Stages;
		};

		uint32_t SetIndex;
		uint64_t LayoutHash;
		std::vector<Binding> Bindings;

		void BuildLayoutHash();
	};

	struct PushConstantBlock
	{
		struct Member
		{
			uint32_t Offset;
			size_t Size;
		};

		uint32_t Offset;
		uint32_t Size;
		EShaderStage Stages;
		std::vector<Member> Members;
	};

	struct BindingRef
	{
		uint32_t SetIndex;
		uint32_t BindingIndex;
	};

	struct PushConstantRef
	{
		uint32_t BlockIndex;
		uint32_t MemberIndex;
	};

	struct ShaderTemplate
	{
		std::vector<BindingSetTemplate> Sets;
		std::vector<PushConstantBlock> PushConstants;
		size_t TotalPushConstantSize = 0;
		uint32_t MaxSet = 0;

		std::unordered_map<uint64_t, BindingRef> BindingLookup;
		std::unordered_map<uint64_t, PushConstantRef> PushConstantLookup;
		std::unordered_map<uint32_t, uint32_t> SetIndexLookup;

		const PushConstantBlock::Member *FindPushConstant(uint64_t nameHash) const;

		const BindingSetTemplate::Binding *FindBinding(uint64_t nameHash) const;

		const BindingSetTemplate *FindSet(uint32_t setIndex) const;

		static ShaderTemplate Build(const struct FShaderReflection &reflection);
	};
} // namespace BHive