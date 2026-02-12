#pragma once

#include "gfx/StorageBuffer.h"
#include "VulkanBackend.h"

namespace BHive
{
	class BHIVE_API VulkanStorageBuffer : public StorageBuffer
	{
	public:
		VulkanStorageBuffer(uint32_t binding, size_t size, const void *data = nullptr);

		VulkanStorageBuffer(size_t size);

		void SetData(const void *data, size_t, uint32_t offset = 0) override;

		NativeHandle GetNativeHandle(uint32_t frame) const override { return Vulkan::Handle::BufferInfo(&mBufferInfo); }

	private:
		vk::raii::Device &mDevice;
		Vulkan::AllocatedBuffer mBuffer;
		uint32_t mBinding{0};
		vk::DescriptorBufferInfo mBufferInfo;
	};

} // namespace BHive
