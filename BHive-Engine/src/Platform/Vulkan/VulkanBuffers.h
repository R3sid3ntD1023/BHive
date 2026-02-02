#pragma once

#include "gfx/Buffers.h"
#include "VulkanCore.h"

namespace BHive
{
	class BHIVE_API VulkanIndexBuffer : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(const uint32_t count, const uint32_t *data);

		virtual uint32_t GetCount() const override { return mCount; }

		virtual void SetData(const void *data, size_t size, uint32_t offset = 0) override;

		operator const vk::raii::Buffer &() const { return mBuffer.Buffer; }

		virtual uintptr_t GetNativeHandle() const override { return 0; }

		operator const vk::Buffer &() const { return mBuffer.Buffer; }

	private:
		vk::raii::Device &mDevice;

		AllocatedVulkanBuffer mBuffer;

		uint32_t mCount;
	};

	class BHIVE_API VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(const size_t size, const void *data);

		virtual void SetData(const void *data, size_t size, uint32_t offset = 0) override;

		virtual void SetLayout(const BufferLayout &layout) override;

		virtual const BufferLayout &GetLayout() const override { return mLayout; }

		operator const vk::Buffer &() const { return mBuffer.Buffer; }

		virtual uintptr_t GetNativeHandle() const override { return 0; }

	private:
		vk::raii::Device &mDevice;

		AllocatedVulkanBuffer mBuffer;

		BufferLayout mLayout{};
	};
} // namespace BHive