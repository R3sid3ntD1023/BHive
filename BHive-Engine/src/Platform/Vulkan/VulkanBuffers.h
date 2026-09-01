#pragma once

#include "VKInterfaces.h"
#include "VulkanMemory.h"
#include "gfx/Buffers.h"

namespace BHive
{
	struct VulkanBuffer : public INativeObject
	{
		~VulkanBuffer();

		void Init(size_t size, const void *data, vk::BufferUsageFlags usage, EBufferLifetime lifeTime);

		const AllocatedBuffer &GetNative(uint32_t frame = 0) const;

		void Upload(const void *data, size_t size, uint32_t offset);

		void ClearData();

		bool NeedsBarrier() const { return mLifeTime == EBufferLifetime::Static; }

	private:
		void InitStatic(size_t size, const void *data, vk::BufferUsageFlags usage);
		void InitDynamic(size_t size, const void *data, vk::BufferUsageFlags usage);

	private:
		std::array<AllocatedBuffer, MAX_FRAMES_IN_FLIGHT> mBuffers;
		std::array<void *, MAX_FRAMES_IN_FLIGHT> mMappedPtrs{};
		EBufferLifetime mLifeTime{};
	};

	//-------------------------Static Buffers----------------------------------//

	class BHIVE_API VulkanIndexBuffer : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(uint32_t count, EBufferLifetime lifeTime, const uint32_t *data);

		void SetData(const void *data, size_t size, uint32_t offset = 0) override;

		void Clear() override;

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

		void SetData(const void *data, size_t size, uint32_t offset = 0) override;

		void Clear() override;

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

		void SetData(const void *data, size_t size, uint32_t offset = 0) override;

		void Clear() override;

		// unused in vulkan
		void BindAtBindingPoint(uint32_t binding) override {}

		NativeHandle GetNativeHandle() const override { return NativeHandle::FromPtr(&mBuffer); }

		bool NeedsBarrier() const override { return mBuffer.NeedsBarrier(); }

	private:
		VulkanBuffer mBuffer;
		size_t mSize{0};
	};
} // namespace BHive