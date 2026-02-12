#pragma once

#include "gfx/Buffers.h"
#include "VulkanBackend.h"

namespace BHive
{
	struct PerFrameBuffer
	{
		Vulkan::AllocatedBuffer Buffer;
		Vulkan::AllocatedBuffer StagingBuffer;
		void *MappedMemory = nullptr;

		void Init(size_t size, vk::BufferUsageFlags usage);

		void SetData(vk::raii::CommandBuffer &cmd, const void *data, size_t size, uint32_t offset, vk::PipelineStageFlags2 flags, vk::AccessFlags2 access);

		void Release();

		NativeHandle GetNativeHandle() const { return Vulkan::Handle::Buffer(&Buffer.Buffer); }

		~PerFrameBuffer();
	};

	
	//-------------------------Static Buffers----------------------------------//

	class BHIVE_API StaticVulkanIndexBuffer : public IndexBuffer
	{
	public:
		StaticVulkanIndexBuffer(uint32_t count);

		~StaticVulkanIndexBuffer();

		virtual uint32_t GetCount() const override { return mCount; }

		virtual void SetData(const void *data, size_t size, uint32_t offset = 0) override;

		virtual NativeHandle GetNativeHandle(uint32_t frame) const override { return mBuffer.GetNativeHandle(); };

	private:
		vk::raii::Device &mDevice;
		PerFrameBuffer mBuffer;
		uint32_t mCount;
	};

	class BHIVE_API StaticVulkanVertexBuffer : public VertexBuffer
	{
	public:
		StaticVulkanVertexBuffer(size_t size);

		~StaticVulkanVertexBuffer();

		virtual void SetData(const void *data, size_t size, uint32_t offset = 0) override;

		virtual void SetLayout(const BufferLayout &layout) override { mLayout = layout; };

		virtual const BufferLayout &GetLayout() const override { return mLayout; }

		virtual NativeHandle GetNativeHandle(uint32_t frame) const override { return mBuffer.GetNativeHandle(); };

	private:
		vk::raii::Device &mDevice;
		PerFrameBuffer mBuffer;
		BufferLayout mLayout{};
	};

	//-------------------------Dynamic Buffers----------------------------------//

	class BHIVE_API DynamicVulkanIndexBuffer : public IndexBuffer
	{
	public:
		DynamicVulkanIndexBuffer(uint32_t count);

		~DynamicVulkanIndexBuffer();

		virtual uint32_t GetCount() const override { return mCount; }

		virtual void SetData(const void *data, size_t size, uint32_t offset = 0) override;

		virtual NativeHandle GetNativeHandle(uint32_t frame) const override
		{
			ASSERT(frame < VulkanBackend::MAX_FRAMES_IN_FLIGHT);
			return mPerFrameBuffer[frame].GetNativeHandle();
		};

	private:
		vk::raii::Device &mDevice;
		std::array<PerFrameBuffer, VulkanBackend::MAX_FRAMES_IN_FLIGHT> mPerFrameBuffer;
		uint32_t mCount;
	};

	class BHIVE_API DynamicVulkanVertexBuffer : public VertexBuffer
	{
	public:
		DynamicVulkanVertexBuffer(const size_t size);

		~DynamicVulkanVertexBuffer();

		virtual void SetData(const void *data, size_t size, uint32_t offset = 0) override;

		virtual void SetLayout(const BufferLayout &layout) override;

		virtual const BufferLayout &GetLayout() const override { return mLayout; }

		virtual NativeHandle GetNativeHandle(uint32_t frame) const override
		{
			ASSERT(frame < VulkanBackend::MAX_FRAMES_IN_FLIGHT);
			return mPerFrameBuffer[frame].GetNativeHandle();
		};


	private:
		vk::raii::Device &mDevice;
		std::array<PerFrameBuffer, VulkanBackend::MAX_FRAMES_IN_FLIGHT> mPerFrameBuffer;
		BufferLayout mLayout{};
		
	};
} // namespace BHive