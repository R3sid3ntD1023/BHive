#pragma once

#include "BufferBase.h"
#include "BufferLayout.h"
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

		operator const vk::raii::Buffer &() const { return mBuffer.Buffer; }

	private:
		uint32_t mCount;
		AllocatedVulkanBuffer mBuffer;
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

		operator const vk::raii::Buffer &() const { return mBuffer.Buffer; }

	private:
		BufferLayout mLayout{};
		AllocatedVulkanBuffer mBuffer;
	};

} // namespace BHive