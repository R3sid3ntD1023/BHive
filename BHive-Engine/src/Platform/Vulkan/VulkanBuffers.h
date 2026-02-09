#pragma once

#include "gfx/Buffers.h"
#include "VulkanCore.h"

namespace BHive
{
	struct PerFrameBuffer
	{
		AllocatedVulkanBuffer Buffer;
		AllocatedVulkanBuffer StagingBuffer;
		void *MappedMemory = nullptr;

		void Init(size_t size, vk::BufferUsageFlags usage);

		void SetData(vk::raii::CommandBuffer &cmd, const void *data, size_t size, uint32_t offset, vk::PipelineStageFlags2 flags, vk::AccessFlags2 access);

		void Release();
	};

	class BHIVE_API IVulkanBufferBase
	{
	public:
		virtual ~IVulkanBufferBase() = default;

		virtual const vk::Buffer& GetBuffer(uint32_t frame) const = 0;
	};

	//-------------------------Static Buffers----------------------------------//

	class BHIVE_API StaticVulkanIndexBuffer : public IndexBuffer, public IVulkanBufferBase
	{
	public:
		StaticVulkanIndexBuffer(uint32_t count);

		~StaticVulkanIndexBuffer();

		virtual uint32_t GetCount() const override { return mCount; }

		virtual void SetData(const void *data, size_t size, uint32_t offset = 0) override;

		virtual uintptr_t GetNativeHandle() const override { return 0; }

		const vk::Buffer &GetBuffer(uint32_t) const{return mBuffer.Buffer.Buffer;}

	private:
		vk::raii::Device &mDevice;
		PerFrameBuffer mBuffer;
		uint32_t mCount;
	};

	class BHIVE_API StaticVulkanVertexBuffer : public VertexBuffer, public IVulkanBufferBase
	{
	public:
		StaticVulkanVertexBuffer(size_t size);

		~StaticVulkanVertexBuffer();

		virtual void SetData(const void *data, size_t size, uint32_t offset = 0) override;

		virtual void SetLayout(const BufferLayout &layout) override { mLayout = layout; };

		virtual const BufferLayout &GetLayout() const override { return mLayout; }

		virtual uintptr_t GetNativeHandle() const override { return 0; }

		const vk::Buffer &GetBuffer(uint32_t) const { return mBuffer.Buffer.Buffer; }

	private:
		vk::raii::Device &mDevice;
		PerFrameBuffer mBuffer;
		BufferLayout mLayout{};
	};

	//-------------------------Dynamic Buffers----------------------------------//

	class BHIVE_API DynamicVulkanIndexBuffer : public IndexBuffer, public IVulkanBufferBase
	{
	public:
		DynamicVulkanIndexBuffer(uint32_t count);

		~DynamicVulkanIndexBuffer();

		virtual uint32_t GetCount() const override { return mCount; }

		virtual void SetData(const void *data, size_t size, uint32_t offset = 0) override;

		const vk::Buffer &GetBuffer(uint32_t frame) const
		{
			ASSERT(frame < VulkanCore::MAX_FRAMES_IN_FLIGHT);
			return mPerFrameBuffer[frame].Buffer.Buffer;
		}

		virtual uintptr_t GetNativeHandle() const override { return 0; }

	private:
	private:
		vk::raii::Device &mDevice;
		std::array<PerFrameBuffer, VulkanCore::MAX_FRAMES_IN_FLIGHT> mPerFrameBuffer;
		uint32_t mCount;
	};

	class BHIVE_API DynamicVulkanVertexBuffer : public VertexBuffer, public IVulkanBufferBase
	{
	public:
		DynamicVulkanVertexBuffer(const size_t size);

		~DynamicVulkanVertexBuffer();

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
		
	private:
		vk::raii::Device &mDevice;
		std::array<PerFrameBuffer, VulkanCore::MAX_FRAMES_IN_FLIGHT> mPerFrameBuffer;
		BufferLayout mLayout{};
		
	};
} // namespace BHive