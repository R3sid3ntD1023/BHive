#pragma once

#include "gfx/Buffers.h"
#include "GLBufferBase.h"

namespace BHive
{
	class GLIndexBuffer : public IndexBuffer, public GLBufferBase
	{
	public:
		GLIndexBuffer(const uint32_t *data, const uint32_t count);
		GLIndexBuffer(const uint32_t count);

		uint32_t GetCount() const { return mCount; }
		uint32_t GetBufferID() const { return mBufferID; }

		void SetData(const void *data, uint64_t size, uint32_t offset = 0);

	private:
		uint32_t mCount;
	};

	class GLVertexBuffer : public VertexBuffer, public GLBufferBase
	{
	public:
		GLVertexBuffer(const float *data, const uint64_t size);
		GLVertexBuffer(const uint64_t size);

		uint32_t GetBufferID() const { return mBufferID; }
		virtual void BindBufferBase(uint32_t binding) const override;

		void SetData(const void *data, uint64_t size, uint32_t offset = 0);
		void SetLayout(const BufferLayout &layout);
		const BufferLayout &GetLayout() const { return mLayout; }

	private:
		BufferLayout mLayout;
	};
} // namespace BHive