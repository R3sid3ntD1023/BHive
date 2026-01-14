#pragma once

#include "VulkanCore.h"

namespace BHive
{
	class BHIVE_API FDescriptorSetLayout
	{
	public:
		FDescriptorSetLayout(const std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding> &bindings);
		FDescriptorSetLayout(const FDescriptorSetLayout &) = delete;
		FDescriptorSetLayout &operator=(const FDescriptorSetLayout &) = delete;
		virtual ~FDescriptorSetLayout() = default;

		const vk::raii::DescriptorSetLayout &GetLayout() const { return mLayout; }

		class Builder
		{
		public:
			Builder() {}

			Builder& AddBinding(uint32_t binding, vk::DescriptorType type, vk::ShaderStageFlags stageFlags, uint32_t count)
			{

				ASSERT(!mBindings.contains(binding), "Binding already in use");

				auto layout = vk::DescriptorSetLayoutBinding(binding, type, count, stageFlags, nullptr);
				mBindings.emplace(binding, layout);
				return *this;
			}

			Ref<FDescriptorSetLayout> Build() const
			{
				return CreateRef<FDescriptorSetLayout>(mBindings); 
			}

		private:
			std::unordered_map< uint32_t, vk::DescriptorSetLayoutBinding> mBindings;

		};

	private:
		const vk::raii::Device& mDevice = VK_NULL_HANDLE;
		vk::raii::DescriptorSetLayout mLayout = VK_NULL_HANDLE;
		std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding> mBindings;
		

		friend class FDescriptorWriter;
	};


	class BHIVE_API FDescriptorPool
	{
	public:
		FDescriptorPool(uint32_t maxSets, const std::vector<vk::DescriptorPoolSize> &poolSizes, vk::DescriptorPoolCreateFlags flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);
		FDescriptorPool(const FDescriptorPool &) = delete;
		FDescriptorPool &operator=(const FDescriptorPool &) = delete;
		virtual ~FDescriptorPool() = default;

		bool AllocateDescriptors(const vk::DescriptorSetLayout &layout, vk::raii::DescriptorSets &sets);

		void ResetPool();

		vk::raii::DescriptorPool &GetPool() { return mPool; }

		class Builder
		{
		public:
			Builder() {}

			Builder&AddPoolSize(vk::DescriptorType type, uint32_t count)
			{
				mPoolSizes.emplace_back(type, count);
				return *this;
			}

			Builder &SetPoolFlags(const vk::DescriptorPoolCreateFlags &flags)
			{
				mFlags = flags;
				return *this;
			}

			Builder &SetMaxSets(uint32_t count)
			{
				mMaxSets = count;
				return *this;
			}

			Ref<FDescriptorPool> Build() const
			{
				return CreateRef<FDescriptorPool>(mMaxSets, mPoolSizes, mFlags); 
			}

		private:
			uint32_t mMaxSets = 1000;
			vk::DescriptorPoolCreateFlags mFlags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet | vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind;
			std::vector<vk::DescriptorPoolSize> mPoolSizes;
				
		};

	private:
		const vk::raii::Device &mDevice = VK_NULL_HANDLE;
		vk::raii::DescriptorPool mPool = VK_NULL_HANDLE;

		friend class FDescriptorWriter;
	};

	class BHIVE_API FDescriptorWriter
	{
	public:
		FDescriptorWriter(const Ref<FDescriptorSetLayout> &layout, const Ref<FDescriptorPool> &pool);

		FDescriptorWriter &WriteBuffer(uint32_t binding, const vk::DescriptorBufferInfo &bufferInfo);

		FDescriptorWriter &WriteImage(uint32_t binding, const vk::DescriptorImageInfo &imageInfo);

		bool Build(vk::raii::DescriptorSets &set);

		void Overwrite(vk::raii::DescriptorSets &set);

	private:
		Ref<FDescriptorSetLayout> mLayout;
		Ref<FDescriptorPool> mPool;
		std::vector<vk::WriteDescriptorSet> mWrites;
	};

} // namespace BHive