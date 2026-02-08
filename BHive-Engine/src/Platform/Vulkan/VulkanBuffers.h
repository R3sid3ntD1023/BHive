#pragma once

#include "gfx/Buffers.h"
#include "VulkanCore.h"

namespace BHive
{
	class BHIVE_API VulkanIndexBuffer : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(const uint32_t count, const uint32_t *data);

		~VulkanIndexBuffer();

		virtual uint32_t GetCount() const override { return mCount; }

		virtual void SetData(const void *data, size_t size, uint32_t offset = 0) override;

		virtual uintptr_t GetNativeHandle() const override { return 0; }

		const vk::Buffer &GetBuffer() const
		{
			return mBuffer.Buffer;
		}

	private:
		vk::raii::Device &mDevice;

		AllocatedVulkanBuffer mBuffer;
		AllocatedVulkanBuffer mStagingBuffer;
		void * mMappedMemory = nullptr;

		uint32_t mCount;
	};

	class BHIVE_API VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(const size_t size, const void *data);

		~VulkanVertexBuffer();

		virtual void SetData(const void *data, size_t size, uint32_t offset = 0) override;

		virtual void SetLayout(const BufferLayout &layout) override;

		virtual const BufferLayout &GetLayout() const override { return mLayout; }

		const vk::Buffer &GetBuffer(uint32_t frame) const
		{
			ASSERT(frame < VulkanCore::MAX_FRAMES_IN_FLIGHT);
			return mPerFrameBuffer[frame].Buffer.Buffer;
		}

		virtual uintptr_t GetNativeHandle() const override { return 0; }

	private:
		struct PerFrameBuffer
		{
			AllocatedVulkanBuffer Buffer;
			AllocatedVulkanBuffer StagingBuffer;
			void *MappedMemory = nullptr;

			void Init(size_t size);

			void SetData(vk::raii::CommandBuffer& cmd, const void *data, size_t size, uint32_t offset);

			void Release();
		};

	private:
		vk::raii::Device &mDevice;
		std::array<PerFrameBuffer, VulkanCore::MAX_FRAMES_IN_FLIGHT> mPerFrameBuffer;
		BufferLayout mLayout{};
		
	};
} // namespace BHive