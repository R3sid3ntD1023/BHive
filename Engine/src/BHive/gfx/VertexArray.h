#pragma once

#include "Buffers.h"

namespace BHive
{
	class VertexArray
	{
	public:
		VertexArray();
		virtual ~VertexArray();

		virtual void Bind() const;
		virtual void UnBind() const;

		virtual void SetIndexBuffer(const Ref<IndexBuffer> &indexbuffer);
		virtual void AddVertexBuffer(const Ref<VertexBuffer> &vertexbuffer);

		virtual const Ref<IndexBuffer> &GetIndexBuffer() const { return mIndexBuffer; }
		virtual void BindBuffersBase(uint32_t binding) const;

	private:
		uint32_t mVertexBufferIndex{0};
		uint32_t mVertexArrayID{0};

		Ref<IndexBuffer> mIndexBuffer;
		std::vector<Ref<VertexBuffer>> mVertexBuffers;
	};
} // namespace BHive