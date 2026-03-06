#pragma once

#include "VulkanCore.h"
#include "gfx/ISetManager.h"

namespace BHive
{
	class VulkanSetManager : public ISetManager
	{
	public:
		VulkanSetManager(vk::raii::Device& device, vk::raii::DescriptorPool& pool, vk::DescriptorSetLayout layout, uint32_t setIndex,
			const FShaderReflection& refl);

		virtual void BindBuffer(uint32_t binding, EResourceType type, const Ref<BufferBase> &buffer) override;

		virtual void BindSampler(uint32_t binding, EResourceType type, const Ref<Texture> &texture) override;

		virtual void Update(uint32_t frame) override;

		virtual NativeHandle GetNativeSet(uint32_t frame) override;

	private:
		vk::raii::Device &mDevice;
		
		uint32_t mSetIndex = 0;

		FShaderReflection mRefl;

		std::vector<vk::raii::DescriptorSet> mSets;

		std::unordered_map<uint32_t, std::pair<vk::DescriptorType, vk::DescriptorBufferInfo>> mLocalBuffers;//bindings

		std::unordered_map < uint32_t, std::pair<vk::DescriptorType, vk::DescriptorImageInfo>> mLocalSamplers; // bindings
	};

	
}