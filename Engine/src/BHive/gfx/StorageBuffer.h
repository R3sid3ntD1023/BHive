#pragma once

#include "core/Core.h"
#include "BufferBase.h"
#include "VulkanCore.h"

namespace BHive
{
	class StorageBuffer : public BufferBase
	{
	public:
		StorageBuffer() = default;
		StorageBuffer(uint32_t binding, size_t size, const void* data = nullptr);
		StorageBuffer(size_t size);

		void SetData(const void *data, size_t, uint32_t offset = 0) override;

		virtual void BindBufferBase(uint32_t binding) const override;

	private:
		vk::raii::Buffer mBuffer = VK_NULL_HANDLE;
		vk::raii::DeviceMemory mMemory = VK_NULL_HANDLE;
	};

} // namespace BHive
