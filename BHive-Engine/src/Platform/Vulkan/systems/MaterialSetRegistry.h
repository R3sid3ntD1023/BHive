#pragma once

#include "core/Core.h"

namespace BHive
{
	class VulkanBackendMaterial;
	class ISetManager;
	class VulkanPipeline;
	struct FSetReflection;

	class MaterialSetRegistry
	{
		struct Entry
		{
			VulkanBackendMaterial *Owner = nullptr;
			uint64_t SetHash = 0;
			uint64_t SetIndex = 0;
			Ref<ISetManager> Manager;
		};

	public:
		Entry &CreateForMaterial(VulkanBackendMaterial *mat, const VulkanPipeline *pipeline);

		Ref<ISetManager> Find(VulkanBackendMaterial *mat) const;

		void UpdatePerFrame(uint32_t frame);

	private:
		void BindMaterialResources(VulkanBackendMaterial *mat, const FSetReflection &refl, ISetManager *mang);

	private:
		std::unordered_map<VulkanBackendMaterial*, Entry> mEntries;
	};

} // namespace BHive