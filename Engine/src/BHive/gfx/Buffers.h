#pragma once

#include "BufferLayout.h"
#include "BufferBase.h"

namespace BHive
{
	class BHIVE_API IndexBuffer : public BufferBase
	{
	public:
		IndexBuffer(const uint32_t *data, const uint32_t count);
		IndexBuffer(const uint32_t count);

		virtual ~IndexBuffer() = default;

		virtual uint32_t GetCount() const { return mCount; }

	private:
		uint32_t mCount;
	};

	class BHIVE_API VertexBuffer : public BufferBase
	{
	public:
		virtual ~VertexBuffer() = default;
		VertexBuffer(const float *data, const uint64_t size);
		VertexBuffer(const uint64_t size);

		virtual void SetLayout(const BufferLayout &layout);
		virtual const BufferLayout &GetLayout() const { return mLayout; }

	private:
		BufferLayout mLayout;
	};

} // namespace BHive