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

		uintptr_t GetNativeHandle() const override { return reinterpret_cast<uintptr_t>(&mBufferInfo); }

		vk::DescriptorBufferInfo GetBufferInfo(uint32_t frame) const;

	private:
		vk::raii::Device &mDevice;
		std::array<Vulkan::AllocatedBuffer, VulkanBackend::MAX_FRAMES_IN_FLIGHT> mBuffer;
		std::array<void *, VulkanBackend::MAX_FRAMES_IN_FLIGHT> mMappedMemory;
		vk::DescriptorBufferInfo mBufferInfo;
		vk::DeviceSize mSize;
	};
} // namespace BHive