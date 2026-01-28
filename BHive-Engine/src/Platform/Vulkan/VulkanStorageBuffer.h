#pragma once

#include "gfx/StorageBuffer.h"
#include "VulkanCore.h"

namespace BHive
{
	class BHIVE_API VulkanStorageBuffer : public StorageBuffer
	{
	public:
		VulkanStorageBuffer(uint32_t binding, size_t size, const void *data = nullptr);

		VulkanStorageBuffer(size_t size);

		void SetData(const void *data, size_t, uint32_t offset = 0) override;

		virtual uintptr_t GetNativeHandle() const { return reinterpret_cast<uintptr_t>(&mBufferInfo); }

	private:
		vk::raii::Device &mDevice;
		AllocatedVulkanBuffer mBuffer;
		uint32_t mBinding{0};
		vk::DescriptorBufferInfo mBufferInfo;
	};

} // namespace BHive
