#pragma once

#include "MemoryAllocator.h"
#include "VulkanCore.h"
#include "gfx/Enumerations.h"

namespace BHive
{
	struct VulkanBuffer
	{
		explicit VulkanBuffer(vk::raii::Device &device);

		VulkanBuffer(const VulkanBuffer &) = delete;
		VulkanBuffer &operator=(const VulkanBuffer &) = delete;
		VulkanBuffer(VulkanBuffer &&other) noexcept;
		VulkanBuffer &operator=(VulkanBuffer &&) = delete;

		~VulkanBuffer();

		void Reset();

		void Initialize(size_t size, vk::BufferUsageFlags usage, EBufferLifetime lifeTime);

		const vk::raii::Buffer &GetNative(uint32_t frame = 0) const;

		vk::DeviceSize GetSize() const { return mSize; }

		void Upload(const void *data, size_t size, uint32_t offset);

		void ClearData();

	private:
		void InitStatic(size_t size, vk::BufferUsageFlags usage);
		void InitDynamic(size_t size, vk::BufferUsageFlags usage);
		void *MapAllocation(uint32_t index);
		void CopyStaticBuffer(vk::DeviceSize size, vk::DeviceSize offset = 0);

	private:
		vk::raii::Device &mDevice;
		std::vector<vk::raii::Buffer> mBuffers;
		std::array<MemoryAllocation, MAX_FRAMES_IN_FLIGHT> mAllocations{};
		vk::DeviceSize mSize = 0;
		uint32_t mBufferCount = 0;
		EBufferLifetime mLifeTime{};
	};
} // namespace BHive