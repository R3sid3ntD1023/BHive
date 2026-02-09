#pragma once

#include "gfx/Buffers.h"
#include "gfx/Shader.h"
#include "gfx/VertexArray.h"
#include "RenderBatch.h"

namespace BHive
{

	template <typename T>
	struct TRenderBatch : public IRenderBatch
	{
		uint32_t *mIndexCurrentPtr = nullptr;
		uint32_t *mIndexBufferPtr = nullptr;

		Ref<IndexBuffer> mIndexBuffer;
		Ref<VertexBuffer> mVertexBuffer;
		Ref<VertexArray> mVertexArray;

		// current vertex and index
		T *mVertexCurrentPtr = nullptr;
		T *mVertexBufferPtr = nullptr;

		uint32_t mIndexCount = 0;
		uint32_t mVertexCount = 0;

		virtual void Init(size_t vcount, size_t icount)
		{
			mVertexBufferPtr = new T[vcount];
			mIndexBufferPtr = new uint32_t[icount];

			mIndexBuffer = IndexBuffer::Create((uint32_t)icount, EBufferUsage::Dynamic);
			mVertexBuffer = VertexBuffer::Create(vcount * sizeof(T), EBufferUsage::Dynamic);
			mVertexBuffer->SetLayout(T::GetLayout());

			mVertexArray = VertexArray::Create({mVertexBuffer}, mIndexBuffer);
		}

		virtual ~TRenderBatch()
		{
			delete[] mVertexBufferPtr;
			delete[] mIndexBufferPtr;
		}

		// sets the index and vertex buffer data
		virtual void Flush()
		{
			mVertexBuffer->SetData(mVertexBufferPtr, GetVertexBufferSize());
			mIndexBuffer->SetData(mIndexBufferPtr, GetIndexBufferSize());
		}

		virtual void StartBatch()
		{
			mIndexCount = 0;
			mVertexCount = 0;
			mVertexCurrentPtr = mVertexBufferPtr;
			mIndexCurrentPtr = mIndexBufferPtr;
		}

		virtual void NextBatch()
		{
			End();
			StartBatch();
		}

		virtual void End()
		{
			auto shader = GetShader();
			shader->Bind();
			Flush();
			shader->UnBind();
		}

		virtual Ref<Shader> GetShader() const = 0;

		size_t GetVertexBufferSize() { return (size_t)((uint8_t *)mVertexCurrentPtr - (uint8_t *)mVertexBufferPtr); }
		size_t GetIndexBufferSize() { return (size_t)((uint8_t *)mIndexCurrentPtr - (uint8_t *)mIndexBufferPtr); }
	};
} // namespace BHive