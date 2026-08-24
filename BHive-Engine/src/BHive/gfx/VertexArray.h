#pragma once

#include "Buffers.h"

namespace BHive
{
	struct FPass;

	class BHIVE_API VertexArray
	{
	public:
		virtual ~VertexArray() = default;

		virtual void SetIndexBuffer(const Ref<IndexBuffer> &indexbuffer) = 0;

		virtual void AddVertexBuffer(const Ref<VertexBuffer> &vertexbuffer) = 0;

		virtual const Ref<IndexBuffer> &GetIndexBuffer() const = 0;

		virtual const std::vector<Ref<VertexBuffer>> &GetVertexBuffers() const = 0;

		void DeclareAccess(FPass &pass, EBufferUsage vbAccess, EBufferUsage ibAccess);

		static Ref<VertexArray> Create();

		static Ref<VertexArray> Create(const std::vector<Ref<VertexBuffer>> &vertex_buffers, const Ref<IndexBuffer> &index_buffer = nullptr);
	};
} // namespace BHive