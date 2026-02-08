#include "DescriptorBuilder.h"

namespace BHive
{
	FDescriptorSetLayout::FDescriptorSetLayout(const std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding> &bindings)
		: mDevice(VulkanCore::GetLogicalDevice()),
		  mBindings(bindings)
	{
		auto number_of_bindings = mBindings.size();
		std::vector<vk::DescriptorSetLayoutBinding> layoutBindings;
		std::vector<vk::DescriptorBindingFlags> binding_flags;

		layoutBindings.reserve(number_of_bindings);
		binding_flags.reserve(number_of_bindings);

		for (const auto &[_, binding] : mBindings)
		{
			layoutBindings.push_back(binding);
			binding_flags.push_back(vk::DescriptorBindingFlagBits::eUpdateAfterBind);
		}

		vk::DescriptorSetLayoutBindingFlagsCreateInfo binding_flags_info(binding_flags);

		vk::DescriptorSetLayoutCreateInfo layout_create_info(vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool, layoutBindings);
		layout_create_info.pNext = &binding_flags_info;
		mLayout = mDevice.createDescriptorSetLayout(layout_create_info);
	}

	FDescriptorPool::FDescriptorPool(uint32_t maxSets, const std::vector<vk::DescriptorPoolSize> &poolSizes, vk::DescriptorPoolCreateFlags flags)
		: mDevice(VulkanCore::GetLogicalDevice())
	{

		vk::DescriptorPoolCreateInfo poolInfo(flags, maxSets, poolSizes);
		mPool = std::move(mDevice.createDescriptorPool(poolInfo));
	}

	bool FDescriptorPool::AllocateDescriptors(const vk::DescriptorSetLayout &layout, vk::raii::DescriptorSets &sets)
	{
		std::vector<vk::DescriptorSetLayout> layouts(VulkanCore::MAX_FRAMES_IN_FLIGHT, layout);
		vk::DescriptorSetAllocateInfo allocInfo(*mPool, layouts);
		auto allocated_sets = vk::raii::DescriptorSets(mDevice, allocInfo);

		if (!allocated_sets.size())
		{
			return false;
		}

		sets = std::move(allocated_sets);
		return true;
	}

	void FDescriptorPool::ResetPool()
	{
		mPool.reset(vk::DescriptorPoolResetFlags());
	}

	FDescriptorWriter::FDescriptorWriter(const Ref<FDescriptorSetLayout> &layout, const Ref<FDescriptorPool> &pool)
		: mLayout(layout),
		  mPool(pool)
	{
	}

	FDescriptorWriter &FDescriptorWriter::WriteBuffer(uint32_t binding, const vk::DescriptorBufferInfo &bufferInfo)
	{
		ASSERT(mLayout->mBindings.contains(binding), "Layout does not contain specified binding");

		auto &bindingDescription = mLayout->mBindings[binding];

		ASSERT(bindingDescription.descriptorCount == 1, "Binding single descriptor info, but binding expects multiple");

		vk::WriteDescriptorSet write({}, binding, 0, bindingDescription.descriptorType, {}, bufferInfo);
		mWrites.push_back(write);
		return *this;
	}

	FDescriptorWriter &FDescriptorWriter::WriteImage(uint32_t binding, const vk::DescriptorImageInfo &imageInfo)
	{
		ASSERT(mLayout->mBindings.contains(binding), "Layout does not contain specified binding");
		auto &bindingDescription = mLayout->mBindings[binding];
		ASSERT(bindingDescription.descriptorCount == 1, "Binding single descriptor info, but binding expects multiple");

		vk::WriteDescriptorSet write({}, binding, 0, bindingDescription.descriptorType, imageInfo, {});
		mWrites.push_back(write);
		return *this;
	}

	bool FDescriptorWriter::Build(vk::raii::DescriptorSets &sets)
	{
		bool success = mPool->AllocateDescriptors(mLayout->GetLayout(), sets);
		if (!success)
		{
			return false;
		}

		for (auto& set : sets)
			Overwrite(set);

		return true;
	}

	void FDescriptorWriter::Overwrite(vk::raii::DescriptorSet &set)
	{
		for (auto &write : mWrites)
		{
			write.dstSet = set;
		}

		mLayout->mDevice.updateDescriptorSets(mWrites, {});
	}

} // namespace BHive
