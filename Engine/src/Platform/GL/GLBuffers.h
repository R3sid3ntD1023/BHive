#pragma once

#include "gfx/Buffers.h"

namespace BHive
{
	class GLIndexBuffer : public IndexBuffer
	{
	public:
		GLIndexBuffer(const uint32_t* data, const uint32_t count);
		GLIndexBuffer(const uint32_t count);
		~GLIndexBuffer();

		void Bind() const;
		void UnBind() const;

		uint32_t GetCount() const { return mCount; }

		void SetData(const void* data, uint64_t size, uint32_t offset = 0);

	private:
		uint32_t mCount;
		uint32_t mIndexBufferID{ 0 };
	};

	class GLVertexBuffer : public VertexBuffer
	{
	public:
		GLVertexBuffer(const float* data, const uint64_t size);
		GLVertexBuffer(const uint64_t size);

		~GLVertexBuffer();

		void Bind() const;
		void UnBind() const;

		void SetData(const void* data, uint64_t size, uint32_t offset = 0);
		void SetLayout(const BufferLayout& layout);
		const BufferLayout& GetLayout() const { return mLayout; }

	private:
		BufferLayout mLayout;
		uint32_t mVertexBufferID{ 0 };
	};
}