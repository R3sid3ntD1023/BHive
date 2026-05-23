#pragma once

#include "VulkanCore.h"
#include "gfx/SetManagerBase.h"


namespace BHive
{
	class VulkanSetManager : public SetManagerBase
	{
	public:
		
	public:
		VulkanSetManager(vk::raii::Device& device, vk::DescriptorPool pool, vk::DescriptorSetLayout layout, uint32_t setIndex,
			const FShaderReflectionLookUp& refl);

		~VulkanSetManager() override = default;

		virtual void SetBuffer(uint32_t binding, const Ref<BufferBase> &buffer) override;

		virtual void SetTexture(uint32_t binding, const Ref<Texture> &texture, uint32_t mip = 0) override;

		virtual void Update(uint32_t frame) override;

		virtual NativeHandle GetNativeSet(uint32_t frame) override;

	private:

		void AllocateSets();

		vk::DescriptorBufferInfo BuildBufferInfo(const FBindingInfo &b) const;

		vk::DescriptorImageInfo BuildImageInfo(const FBindingInfo &b) const;

	private:
		vk::raii::Device& mDevice;

		vk::DescriptorPool mPool;

		vk::DescriptorSetLayout mLayout;
		
		vk::raii::DescriptorSets mSets = VK_NULL_HANDLE;

		
	};

	
}