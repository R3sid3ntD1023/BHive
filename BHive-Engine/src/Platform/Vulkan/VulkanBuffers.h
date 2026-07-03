#pragma once

#include "gfx/Buffers.h"
#include "VulkanMemory.h"

namespace BHive
{
	struct FBufferUploadInfo
	{
		const void *data;
		size_t size;
		uint32_t offset;
	};

	struct VulkanStaticBuffer
	{
		AllocatedBuffer Buffer;
		AllocatedBuffer StagingBuffer;

		void Init(size_t size, vk::BufferUsageFlags usage);

		void Upload(vk::raii::CommandBuffer &cmd, const FBufferUploadInfo& up);

		~VulkanStaticBuffer();
	};

	struct VulkanPerFrameHostBuffer
	{
		std::array<AllocatedBuffer, MAX_FRAMES_IN_FLIGHT> Buffers;

		void Init(size_t size, vk::BufferUsageFlags usage);

		void Init(const void* data, size_t size, vk::BufferUsageFlags usage);

		void Upload(uint32_t frame, const FBufferUploadInfo& up);

		vk::Buffer GetBuffer(uint32_t frame) const { return Buffers.at(frame).GetBuffer(); }

		~VulkanPerFrameHostBuffer();
	};

	
	//-------------------------Static Buffers----------------------------------//

	class BHIVE_API StaticVulkanIndexBuffer : public IndexBuffer
	{
	public:
		StaticVulkanIndexBuffer(const uint32_t* data, uint32_t count);

		virtual uint32_t GetCount() const override { return mCount; }

		virtual NativeHandle GetNativeHandle(uint32_t frame) const override;

	private:
		VulkanStaticBuffer mBuffer;
		uint32_t mCount;
	};

	class BHIVE_API StaticVulkanVertexBuffer : public VertexBuffer
	{
	public:
		StaticVulkanVertexBuffer(const void* data, size_t size);

		virtual void SetLayout(const BufferLayout &layout) override { mLayout = layout; };

		virtual const BufferLayout &GetLayout() const override { return mLayout; }

		virtual NativeHandle GetNativeHandle(uint32_t frame) const override;

	private:
		VulkanStaticBuffer mBuffer;
		BufferLayout mLayout{};
	};

	//-------------------------Dynamic Buffers----------------------------------//

	class BHIVE_API DynamicVulkanIndexBuffer : public IndexBuffer
	{
	public:
		DynamicVulkanIndexBuffer(const uint32_t *data, uint32_t count);

		virtual uint32_t GetCount() const override { return mCount; }

		virtual NativeHandle GetNativeHandle(uint32_t frame) const override;

	private:
		VulkanPerFrameHostBuffer mPerFrameBuffer;
		uint32_t mCount;
	};

	class BHIVE_API DynamicVulkanVertexBuffer : public VertexBuffer
	{
	public:
		DynamicVulkanVertexBuffer(const void *data, const size_t size);

		virtual void SetLayout(const BufferLayout &layout) override;

		virtual const BufferLayout &GetLayout() const override { return mLayout; }

		virtual NativeHandle GetNativeHandle(uint32_t frame) const override;

	private:
		VulkanPerFrameHostBuffer mPerFrameBuffer;
		BufferLayout mLayout{};
		
	};

	

	class BHIVE_API VulkanGPUBuffer : public GPUBuffer
	{
	public:
		VulkanGPUBuffer(size_t size, EBufferType type, const void *data);

		//unused in vulkan
		void BindAtBindingPoint(uint32_t binding) override {}

		NativeHandle GetNativeHandle(uint32_t frame) const override;

	private:
		VulkanPerFrameHostBuffer mPerFrameBuffer;
		size_t mSize{0};
	};
} // namespace BHive