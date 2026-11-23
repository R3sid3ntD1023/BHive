#pragma once

#include "gfx/VertexArray.h"

namespace BHive
{
	class GLVertexArray : public VertexArray
	{
	public:
		GLVertexArray();

		GLVertexArray(const std::vector<VertexBufferPtr> &vertex_buffers, IndexBufferPtr index_buffer = {});

		void SetIndexBuffer(IndexBufferPtr indexbuffer) override;

		void AddVertexBuffer(VertexBufferPtr vertexbuffer) override;

		void Bind() const;

		IndexBufferPtr GetIndexBuffer() const override { return mIndexBuffer; }

		const std::vector<VertexBufferPtr> &GetVertexBuffers() const override { return mVertexBuffers; }

	private:
		void CreateBindingsAndAttributes(VertexBufferPtr vertexbuffer);

	private:
		IndexBufferPtr mIndexBuffer;

		std::vector<VertexBufferPtr> mVertexBuffers;

		uint32_t mVertexAttributeIndex = 0;

		uint32_t mRendererID = 0;
	};
} // namespace BHive