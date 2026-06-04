#pragma once

#include "gfx/Buffers.h"
#include "gfx/VertexArray.h"

namespace BHive
{
	template <typename T>
	struct VertexBatchBuffer 
	{
		VertexBatchBuffer(size_t maxVerts, size_t maxIndices, bool useIndexBuffer)
			: mUseIndexBuffer(useIndexBuffer)
		{
			mVertexCPU.resize(maxVerts);
			if (useIndexBuffer)
				mIndexCPU.resize(maxIndices);

			if (useIndexBuffer)
				mIndexBuffer = IndexBuffer::Create((uint32_t)maxIndices, EBufferUsageType::Dynamic);

			mVertexBuffer = VertexBuffer::Create(maxVerts * sizeof(T), EBufferUsageType::Dynamic);
			mVertexBuffer->SetLayout(T::GetLayout());

			mVertexArray = VertexArray::Create({mVertexBuffer}, mIndexBuffer);
		}

		T* PushVertex() { return &mVertexCPU[mVertexCount++];}

		uint32_t* PushIndex() { return &mIndexCPU[mIndexCount++];}

		// sets the index and vertex buffer data
		void Upload()
		{
			mVertexBuffer->SetData(mVertexCPU.data(), mVertexCount * sizeof(T));

			if (mUseIndexBuffer)
				mIndexBuffer->SetData(mIndexCPU.data(), mIndexCount * sizeof(uint32_t));
		}

		void Reset()
		{
			mIndexCount = 0;
			mVertexCount = 0;
		}

		VertexArray *GetVAO() const { return mVertexArray.get(); }
		uint32_t GetIndexCount() const { return mIndexCount; }
		uint32_t GetVertexCount() const { return mVertexCount; }

	private:
		bool mUseIndexBuffer{false};

		std::vector<T> mVertexCPU{};
		std::vector<uint32_t> mIndexCPU{};

		uint32_t mIndexCount = 0;
		uint32_t mVertexCount = 0;

		Ref<IndexBuffer> mIndexBuffer;
		Ref<VertexBuffer> mVertexBuffer;
		Ref<VertexArray> mVertexArray;
	};
} // namespace BHive