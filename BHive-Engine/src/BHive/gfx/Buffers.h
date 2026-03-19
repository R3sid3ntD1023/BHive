#pragma once

#include "BufferBase.h"
#include "BufferLayout.h"

namespace BHive
{
	
	class BHIVE_API IndexBuffer : public BufferBase
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual uint32_t GetCount() const = 0;

		static Ref<IndexBuffer> Create(const uint32_t count,  EBufferUsageType usage = EBufferUsageType::Static);
	};

	class BHIVE_API VertexBuffer : public BufferBase
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void SetLayout(const BufferLayout &layout) = 0;

		virtual const BufferLayout &GetLayout() const = 0;

		static Ref<VertexBuffer> Create(const uint64_t size,  EBufferUsageType usage = EBufferUsageType::Static);
	};

	
	class BHIVE_API GPUBuffer : public BufferBase
	{
	public:
		virtual ~GPUBuffer() = default;

		static Ref<GPUBuffer> Create(uint32_t binding, size_t size, EBufferType type, const void *data);

		static Ref<GPUBuffer> Create(uint32_t binding, size_t size, EBufferType type);
	};


} // namespace BHive