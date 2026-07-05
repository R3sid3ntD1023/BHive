#pragma once

#include "gfx/Buffers.h"
#include "VulkanMemory.h"
#include "VKInterfaces.h"

namespace BHive
{
	struct FBufferUploadInfo
	{
		const void *data;
		size_t size;
		uint32_t offset;
	};

	struct VulkanBuffer : public INativeObject
	{
		virtual ~VulkanBuffer() = default;

		virtual const AllocatedBuffer &GetNative(uint32_t frame = 0) const = 0;

		virtual void Upload(vk::CommandBuffer cmd, uint32_t frame, const FBufferUploadInfo &up) = 0;
	};

	struct VulkanStaticBuffer : public VulkanBuffer
	{
		AllocatedBuffer Buffer;
		AllocatedBuffer StagingBuffer;

		void Init(size_t size, vk::BufferUsageFlags usage);

		void Upload(vk::CommandBuffer cmd, uint32_t frame, const FBufferUploadInfo &up) override;

		const AllocatedBuffer& GetNative(uint32_t frame) const override { return Buffer; }

		~VulkanStaticBuffer();
	};

	struct VulkanPerFrameHostBuffer : public VulkanBuffer
	{
		std::array<AllocatedBuffer, MAX_FRAMES_IN_FLIGHT> Buffers;

		void Init(size_t size, vk::BufferUsageFlags usage);

		void Init(const void* data, size_t size, vk::BufferUsageFlags usage);

		void Upload(vk::CommandBuffer cmd, uint32_t frame, const FBufferUploadInfo &up) override;

		const AllocatedBuffer& GetNative(uint32_t frame) const override { return Buffers.at(frame); }

		~VulkanPerFrameHostBuffer();
	};


	//-------------------------Static Buffers----------------------------------//

	class BHIVE_API StaticVulkanIndexBuffer : public IndexBuffer
	{
	public:
		StaticVulkanIndexBuffer(const uint32_t* data, uint32_t count);

		uint32_t GetCount() const override { return mCount; }

		NativeHandle GetNativeHandle() const override { return NativeHandle::FromPtr(&mBuffer); }

		bool NeedsBarrier() const override { return true; }

	private:
		VulkanStaticBuffer mBuffer;
		uint32_t mCount;
	};

	class BHIVE_API StaticVulkanVertexBuffer : public VertexBuffer
	{
	public:
		StaticVulkanVertexBuffer(const void* data, size_t size);

		void SetLayout(const BufferLayout &layout) override { mLayout = layout; };

		const BufferLayout &GetLayout() const override { return mLayout; }

		NativeHandle GetNativeHandle() const override { return NativeHandle::FromPtr(&mBuffer); }

		bool NeedsBarrier() const override { return true; }

	private:
		VulkanStaticBuffer mBuffer;
		BufferLayout mLayout{};
	};

	//-------------------------Dynamic Buffers----------------------------------//

	class BHIVE_API DynamicVulkanIndexBuffer : public IndexBuffer
	{
	public:
		DynamicVulkanIndexBuffer(const uint32_t *data, uint32_t count);

		uint32_t GetCount() const override { return mCount; }

		NativeHandle GetNativeHandle() const override { return NativeHandle::FromPtr(&mPerFrameBuffer); }

	private:
		VulkanPerFrameHostBuffer mPerFrameBuffer;
		uint32_t mCount;
	};

	class BHIVE_API DynamicVulkanVertexBuffer : public VertexBuffer
	{
	public:
		DynamicVulkanVertexBuffer(const void *data, const size_t size);

		void SetLayout(const BufferLayout &layout) override;

		const BufferLayout &GetLayout() const override { return mLayout; }

		NativeHandle GetNativeHandle() const override { return NativeHandle::FromPtr(&mPerFrameBuffer); }

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

		NativeHandle GetNativeHandle() const override { return NativeHandle::FromPtr(&mPerFrameBuffer); }

	private:
		VulkanPerFrameHostBuffer mPerFrameBuffer;
		size_t mSize{0};
	};
} // namespace BHive