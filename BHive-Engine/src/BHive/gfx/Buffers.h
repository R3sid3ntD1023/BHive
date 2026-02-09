#pragma once

#include "BufferBase.h"
#include "BufferLayout.h"

namespace BHive
{
	enum class EBufferUsage
	{
		Static,
		Dynamic
	};

	class BHIVE_API IndexBuffer : public BufferBase
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual uint32_t GetCount() const = 0;

		static Ref<IndexBuffer> Create(const uint32_t count,  EBufferUsage usage = EBufferUsage::Static);
	};

	class BHIVE_API VertexBuffer : public BufferBase
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void SetLayout(const BufferLayout &layout) = 0;

		virtual const BufferLayout &GetLayout() const = 0;

		static Ref<VertexBuffer> Create(const uint64_t size,  EBufferUsage usage = EBufferUsage::Static);
	};

} // namespace BHive