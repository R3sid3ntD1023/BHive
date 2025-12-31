#pragma once

#include "BufferBase.h"
#include "VulkanCore.h"

namespace BHive
{
	class BHIVE_API UniformBuffer : public BufferBase
	{
	public:
		UniformBuffer(uint32_t binding, uint64_t size, const void *data = nullptr);

		void SetData(const void *data, size_t size, uint32_t offset = 0);

		void WriteDescriptor(const vk::raii::DescriptorSet &set) const;

		uint32_t GetBinding() const { return mBinding; }

		uintptr_t GetNativeHandle() const override { return reinterpret_cast<uintptr_t>(&mBufferInfo); }

	private:
		AllocatedVulkanBuffer mBuffer;
		uint32_t mBinding{0};
		vk::DescriptorBufferInfo mBufferInfo;
	};
} // namespace BHive