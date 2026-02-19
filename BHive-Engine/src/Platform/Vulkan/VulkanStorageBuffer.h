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

		~VulkanStorageBuffer();

		void SetData(const void *data, size_t, uint32_t offset = 0) override;

		NativeHandle GetNativeHandle(uint32_t frame) const override;

	private:
		vk::raii::Device &mDevice;
		std::array<Vulkan::AllocatedBuffer, VulkanBackend::MAX_FRAMES_IN_FLIGHT> mBuffer;
		std::array<vk::DescriptorBufferInfo, VulkanBackend::MAX_FRAMES_IN_FLIGHT> mBufferInfo;
		std::array<void *, VulkanBackend::MAX_FRAMES_IN_FLIGHT> mMappedMemory{nullptr};

		uint32_t mBinding{0};
		uint32_t mSize{0};
		
	};

} // namespace BHive
