#pragma once
#include "core/Core.h"
#include "BufferLayout.h"

namespace BHive
{
	class IndexBuffer
	{
	public:
		
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual uint32_t GetCount() const = 0;

		virtual void SetData(const void* data, uint64_t size, uint32_t offset = 0) = 0;

		static Ref<IndexBuffer> Create(const uint32_t* data, const uint32_t count);
		static Ref<IndexBuffer> Create(const uint32_t count);
	};

	class BHIVE VertexBuffer
	{
	public:
		

		virtual ~VertexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual void SetData(const void* data, uint64_t size, uint32_t offset = 0) = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;
		virtual const BufferLayout& GetLayout() const = 0;

		static Ref<VertexBuffer> Create(const float* data, const uint64_t size);
		static Ref<VertexBuffer> Create(const uint64_t size);
	};
}