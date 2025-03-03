#pragma once

#include "gfx/VertexArray.h"

namespace BHive
{
	class GLVertexArray : public VertexArray
	{
	public:
		GLVertexArray();
		~GLVertexArray();

		void Bind() const;
		void UnBind() const;

		void SetIndexBuffer(const Ref<IndexBuffer> &indexbuffer);
		void AddVertexBuffer(const Ref<VertexBuffer> &vertexbuffer);
		virtual void BindBuffersBase(uint32_t binding) const override;

		const Ref<IndexBuffer> &GetIndexBuffer() const { return mIndexBuffer; }

	private:
		uint32_t mVertexBufferIndex{0};
		uint32_t mVertexArrayID{0};

		Ref<IndexBuffer> mIndexBuffer;
		std::vector<Ref<VertexBuffer>> mVertexBuffers;
	};
} // namespace BHive