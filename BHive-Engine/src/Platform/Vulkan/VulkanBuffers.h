#pragma once

#include "gfx/Buffers.h"
#include "VulkanMemory.h"
#include "VKInterfaces.h"

namespace BHive
{
	struct FBufferUploadInfo
	{
		size_t size = 0;
		uint32_t offset = 0;
		const void *data = nullptr;
	};

	struct VulkanBuffer : public INativeObject
	{
		~VulkanBuffer();

		void Init(size_t size, const void* data, vk::BufferUsageFlags usage, EBufferLifetime lifeTime);

		const AllocatedBuffer &GetNative(uint32_t frame = 0) const;

		void Upload(const FBufferUploadInfo &up);

		bool NeedsBarrier() const { return mLifeTime == EBufferLifetime::Static; }

	private:
		void InitStatic(size_t size, const void* data, vk::BufferUsageFlags usage);
		void InitDynamic(size_t size, const void* data, vk::BufferUsageFlags usage);

	private:
		
		std::array<AllocatedBuffer, MAX_FRAMES_IN_FLIGHT> mBuffers;
		EBufferLifetime mLifeTime{};
	};


	//-------------------------Static Buffers----------------------------------//

	class BHIVE_API VulkanIndexBuffer : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(uint32_t count, EBufferLifetime lifeTime, const uint32_t *data);

		uint32_t GetCount() const override { return mCount; }

		NativeHandle GetNativeHandle() const override { return NativeHandle::FromPtr(&mBuffer); }

		bool NeedsBarrier() const override { return mBuffer.NeedsBarrier(); }

	private:
		VulkanBuffer mBuffer;
		uint32_t mCount;
	};

	class BHIVE_API VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(size_t size, EBufferLifetime lifeTime, const void *data);

		void SetLayout(const BufferLayout &layout) override { mLayout = layout; };

		const BufferLayout &GetLayout() const override { return mLayout; }

		NativeHandle GetNativeHandle() const override { return NativeHandle::FromPtr(&mBuffer); }

		bool NeedsBarrier() const override { return mBuffer.NeedsBarrier(); }

	private:
		VulkanBuffer mBuffer;
		BufferLayout mLayout{};
	};

	class BHIVE_API VulkanGeneralBuffer : public GeneralBuffer
	{
	public:
		VulkanGeneralBuffer(size_t size, EBufferType type, EBufferLifetime lifeTime, const void *data);

		//unused in vulkan
		void BindAtBindingPoint(uint32_t binding) override {}

		NativeHandle GetNativeHandle() const override { return NativeHandle::FromPtr(&mBuffer); }

		bool NeedsBarrier() const override { return mBuffer.NeedsBarrier(); }

	private:
		VulkanBuffer mBuffer;
		size_t mSize{0};
	};
} // namespace BHive