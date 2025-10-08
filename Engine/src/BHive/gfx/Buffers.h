#pragma once

#include "BufferLayout.h"
#include "BufferBase.h"
#include "VulkanCore.h"

namespace BHive
{
	class BHIVE_API IndexBuffer : public BufferBase
	{
	public:
		IndexBuffer(const uint32_t *data, const uint32_t count);
		IndexBuffer(const uint32_t count);

		virtual ~IndexBuffer() = default;

		virtual uint32_t GetCount() const { return mCount; }

		virtual void SetData(const void *data, uint64_t size, uint32_t offset = 0);

		operator const vk::raii::Buffer&() const { return mBuffer; }

	private:
		uint32_t mCount;
		vk::raii::Buffer mBuffer = VK_NULL_HANDLE;
		vk::raii::DeviceMemory mBufferMemory = VK_NULL_HANDLE;
	};

	class BHIVE_API VertexBuffer : public BufferBase
	{
	public:
		virtual ~VertexBuffer() = default;
		VertexBuffer(const float *data, const uint64_t size);
		VertexBuffer(const uint64_t size);

		virtual void SetData(const void *data, uint64_t size, uint32_t offset = 0);
		virtual void SetLayout(const BufferLayout &layout);
		virtual const BufferLayout &GetLayout() const { return mLayout; }
		virtual void BindBufferBase(uint32_t binding) const override;

		operator const vk::raii::Buffer &() const { return mBuffer; }

	private:
		BufferLayout mLayout{};
		vk::raii::Buffer mBuffer = VK_NULL_HANDLE;
		vk::raii::DeviceMemory mMemory = VK_NULL_HANDLE;
	};

} // namespace BHive