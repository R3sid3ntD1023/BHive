#pragma once

#include "Buffers.h"

namespace BHive
{
	class BHIVE_API VertexArray
	{
	public:
		virtual ~VertexArray() = default;

		virtual void Bind() const = 0;

		virtual void UnBind() const = 0;

		virtual void SetIndexBuffer(const Ref<IndexBuffer> &indexbuffer) = 0;

		virtual void AddVertexBuffer(const Ref<VertexBuffer> &vertexbuffer) = 0;

		virtual const Ref<IndexBuffer> &GetIndexBuffer() const = 0;

		virtual const std::vector<Ref<VertexBuffer>> &GetVertexBuffers() const = 0;

		static Ref<VertexArray> Create();

		static Ref<VertexArray> Create(const std::vector<Ref<VertexBuffer>>& vertex_buffers, const Ref<IndexBuffer> &index_buffer = nullptr);
	};
} // namespace BHive