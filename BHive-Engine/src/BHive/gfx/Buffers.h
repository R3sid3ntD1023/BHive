#pragma once

#include "BufferBase.h"
#include "BufferLayout.h"

namespace BHive
{
	enum class EBufferType
	{
		UniformBuffer = 0,
		StorageBuffer = 1 << 0,
		IndirectBuffer = 2 << 0
	};

	enum class EBufferUsageType
	{
		Static,
		Dynamic
	};

	struct FBufferCreateInfo
	{
		size_t Size;
		EBufferUsageType Usage;
		EBufferType Type;
	};

	class BHIVE_API IndexBuffer : public BufferBase
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual uint32_t GetCount() const = 0;

		static Ref<IndexBuffer> Create(const uint32_t count, EBufferUsageType usage = EBufferUsageType::Static, const uint32_t *data = nullptr);
	};

	class BHIVE_API VertexBuffer : public BufferBase
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void SetLayout(const BufferLayout &layout) = 0;

		virtual const BufferLayout &GetLayout() const = 0;

		static Ref<VertexBuffer> Create(const uint64_t size, EBufferUsageType usage = EBufferUsageType::Static , const void *data = nullptr);
	};

	
	class BHIVE_API GPUBuffer : public BufferBase
	{
	public:
		virtual ~GPUBuffer() = default;

		virtual void BindAtBindingPoint(uint32_t binding) = 0;

		static Ref<GPUBuffer> Create(size_t size, EBufferType type, const void *data = nullptr);
	};


} // namespace BHive