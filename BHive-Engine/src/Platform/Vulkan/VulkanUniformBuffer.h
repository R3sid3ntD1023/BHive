#pragma once

#include "gfx/UniformBuffer.h"
#include "VulkanBackend.h"

namespace BHive
{
	class BHIVE_API VulkanUniformBuffer : public UniformBuffer
	{
	public:
		VulkanUniformBuffer(uint32_t binding, uint64_t size, const void *data = nullptr);

		~VulkanUniformBuffer();

		void SetData(const void *data, size_t size, uint32_t offset = 0) override;

		NativeHandle GetNativeHandle(uint32_t frame) const override;

	private:
		vk::raii::Device &mDevice;
		std::array<Vulkan::AllocatedBuffer, VulkanBackend::MAX_FRAMES_IN_FLIGHT> mBuffer;
		std::array<void *, VulkanBackend::MAX_FRAMES_IN_FLIGHT> mMappedMemory;
		vk::DeviceSize mSize;
		std::array<vk::DescriptorBufferInfo, VulkanBackend::MAX_FRAMES_IN_FLIGHT> mBufferInfos;
	};
} // namespace BHive