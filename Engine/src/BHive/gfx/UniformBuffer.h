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

		uint32_t GetBinding() const { return mBinding; }

		operator const vk::raii::Buffer&() const { return mBuffer; }

	private:
		vk::raii::Buffer mBuffer = VK_NULL_HANDLE;
		vk::raii::DeviceMemory mMemory = VK_NULL_HANDLE;
		uint32_t mBinding{0};
	};
} // namespace BHive