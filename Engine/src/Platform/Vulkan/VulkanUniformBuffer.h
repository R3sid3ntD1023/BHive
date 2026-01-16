#pragma once

#include "gfx/UniformBuffer.h"
#include "gfx/VulkanCore.h"

namespace BHive
{
	class BHIVE_API VulkanUniformBuffer : public UniformBuffer
	{
	public:
		VulkanUniformBuffer(uint32_t binding, uint64_t size, const void *data = nullptr);

		void SetData(const void *data, size_t size, uint32_t offset = 0) override;

		uintptr_t GetNativeHandle() const override { return reinterpret_cast<uintptr_t>(&mBufferInfo); }

	private:
		vk::raii::Device &mDevice;
		AllocatedVulkanBuffer mBuffer;
		vk::DescriptorBufferInfo mBufferInfo;
	};
} // namespace BHive