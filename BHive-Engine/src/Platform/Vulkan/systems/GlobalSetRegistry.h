#pragma once

#include "core/Core.h"

namespace BHive
{

	class ISetManager;
	class VulkanPipeline;
	struct FShaderReflectionLookUp;

	struct GlobalSetKey
	{
		uint64_t Hash = 0;

		uint64_t SetIndex = 0;
	};

	struct GlobalSetEntry
	{
		GlobalSetKey Key;
		Ref<ISetManager> Manager;
	};

	class GlobalSetRegistry
	{
	public:
		GlobalSetEntry &EnsureGlobalSet(const VulkanPipeline &pipeline, uint32_t setIndex);

		Ref<ISetManager> Find(uint64_t setHash) const;

		void UpdatePerFrame(uint32_t frame);

	private:
		void BindGlobalResources(const FShaderReflectionLookUp &refl, ISetManager *mang, uint32_t setIndex, const std::string &shaderName);

		std::unordered_map<uint64_t, GlobalSetEntry> mEntries;
	};

} // namespace BHive