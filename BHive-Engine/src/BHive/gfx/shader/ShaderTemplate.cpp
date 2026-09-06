#include "ShaderTemplate.h"
#include "ShaderReflection.h"
#include "core/utils/Hash.h"

namespace BHive
{
	void BindingSetTemplate::BuildLayoutHash()
	{
		LayoutHash = 0;
		for (const auto &binding : Bindings)
		{
			BHive::utils::HashCombine(LayoutHash, binding.Binding, (uint32_t)binding.Type, (uint32_t)binding.ArraySize, (uint32_t)binding.Stages);
		}
	}

	const PushConstantBlock::Member *ShaderTemplate::FindPushConstant(uint64_t nameHash) const
	{
		auto it = PushConstantLookup.find(nameHash);
		if (it == PushConstantLookup.end())
		{
			return nullptr;
		}

		const auto &ref = it->second;
		return &(PushConstants.at(ref.BlockIndex).Members.at(ref.MemberIndex));
	}

	const BindingSetTemplate::Binding *ShaderTemplate::FindBinding(uint64_t nameHash) const
	{
		auto it = BindingLookup.find(nameHash);
		if (it == BindingLookup.end())
		{
			return nullptr;
		}

		const auto &ref = it->second;
		return &(Sets.at(ref.SetIndex).Bindings.at(ref.BindingIndex));
	}

	const BindingSetTemplate *ShaderTemplate::FindSet(uint32_t setIndex) const
	{
		return SetIndexLookup.contains(setIndex) ? &Sets.at(SetIndexLookup.at(setIndex)) : nullptr;
	}

	ShaderTemplate ShaderTemplate::Build(const FShaderReflection &reflection)
	{
		ShaderTemplate temp{};

		auto &sets = temp.Sets;
		auto &bindingTable = temp.BindingLookup;
		auto &setTable = temp.SetIndexLookup;
		auto &constants = temp.PushConstants;
		auto &constantsTable = temp.PushConstantLookup;
		auto &constantsSize = temp.TotalPushConstantSize;
		auto &maxSet = temp.MaxSet;

		for (const auto &[setIndex, set] : reflection.Sets)
		{
			const auto &samplers = set.Samplers;
			const auto &uniformBuffers = set.UniformBuffers;
			const auto &storageBuffers = set.StorageBuffers;

			auto &setTemplate = sets.emplace_back();
			setTemplate.SetIndex = setIndex;

			for (const auto &[name, binding] : samplers)
			{
				auto nameHash = utils::ComputeHash<std::string_view>(name);
				auto &setBinding = setTemplate.Bindings.emplace_back((uint32_t)binding.Binding, binding.Type, 0u, (uint32_t)binding.ArraySize, binding.Stages);
				bindingTable[nameHash] = BindingRef{(uint32_t)sets.size() - 1, (uint32_t)setTemplate.Bindings.size() - 1};
			}

			for (auto &[name, binding] : uniformBuffers)
			{
				auto nameHash = utils::ComputeHash<std::string_view>(name);
				auto &setBinding = setTemplate.Bindings.emplace_back((uint32_t)binding.Binding, binding.Type, (uint32_t)binding.Size, 0u, binding.Stages);
				bindingTable[nameHash] = BindingRef{(uint32_t)sets.size() - 1, (uint32_t)setTemplate.Bindings.size() - 1};
			}

			for (auto &[name, binding] : storageBuffers)
			{
				auto nameHash = utils::ComputeHash<std::string_view>(name);
				auto &setBinding = setTemplate.Bindings.emplace_back((uint32_t)binding.Binding, binding.Type, (uint32_t)binding.Size, 0u, binding.Stages);
				bindingTable[nameHash] = BindingRef{(uint32_t)sets.size() - 1, (uint32_t)setTemplate.Bindings.size() - 1};
			}

			setTable[setIndex] = (uint32_t)sets.size() - 1;
			setTemplate.BuildLayoutHash();

			maxSet = std::max(setIndex, maxSet);
		}

		for (const auto &constant : reflection.PushConstants)
		{
			auto &pcTemplate = constants.emplace_back((uint32_t)constant.Offset, (uint32_t)constant.Size, constant.Stages);
			pcTemplate.Members.reserve(constant.Members.size());

			for (const auto &[memberName, member] : constant.Members)
			{
				auto nameHash = utils::ComputeHash<std::string_view>(memberName);
				auto &memberTemplate = pcTemplate.Members.emplace_back((uint32_t)member.Offset, (uint32_t)member.Size);

				constantsTable[nameHash] = PushConstantRef{(uint32_t)constants.size() - 1, (uint32_t)pcTemplate.Members.size() - 1};
			}

			constantsSize += constant.Size;
		}

		return temp;
	}
} // namespace BHive