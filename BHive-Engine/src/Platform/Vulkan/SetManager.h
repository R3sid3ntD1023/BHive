#pragma once

#include "VulkanCore.h"
#include "gfx/shader/ShaderReflection.h"

namespace BHive
{
	class SetManager
	{
	public:
		SetManager(const FSetReflection &set, uint32_t setIndex);

		void Init(vk::raii::Device& device, vk::raii::DescriptorPool &pool, vk::DescriptorSetLayout layout);

		void BindBuffer(uint32_t binding, vk::DescriptorType type,  vk::DescriptorBufferInfo buffer);

		void BindSampler(uint32_t binding, vk::DescriptorImageInfo imageInfo);

		vk::DescriptorSet Get(uint32_t frame) const { return *mSets[frame]; }

		void Update(uint32_t frame, vk::raii::Device &device);

	private:
		std::vector<vk::raii::DescriptorSet> mSets;
		uint32_t mSetIndex = 0;
		std::unordered_map<uint32_t, std::pair<vk::DescriptorType, vk::DescriptorBufferInfo>> mLocalBuffers;//bindings
		std::unordered_map<uint32_t, vk::DescriptorImageInfo> mLocalSamplers;//bindings
		FSetReflection mTargetSet;
	};

	class GlobalSetSystem
	{
	public:
		void Register(uint64_t hash, Scope<SetManager>& manager);

		SetManager *Get(uint64_t hash) const;

		bool Contains(uint64_t hash) const { return mGlobalManagers.contains(hash); }

	private:
		std::unordered_map<uint64_t, Scope<SetManager>> mGlobalManagers;

	};
}