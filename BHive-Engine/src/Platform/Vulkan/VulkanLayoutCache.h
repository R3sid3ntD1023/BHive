#pragma once

#include "VulkanCore.h"

namespace BHive
{
	struct BindingSetTemplate;

	class VulkanLayoutCache
	{
	public:
		vk::DescriptorSetLayout GetOrCreate(const BindingSetTemplate &setTemplate);

		vk::DescriptorSetLayout GetEmptyLayout();

		void BuildDescriptorSetLayout(const BindingSetTemplate &setTemplate);

	private:
		std::unordered_map<uint64_t, vk::raii::DescriptorSetLayout> mCache;
	};
} // namespace BHive