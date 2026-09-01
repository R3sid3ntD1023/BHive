#pragma once

#include "gfx/Buffers.h"
#include "gfx/factories/GFXFactories.h"
#include "gfx/registries/Handles.h"

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
				mIndexBuffer = BufferFactory::CreateIndexBuffer((uint32_t)maxIndices, EBufferLifetime::Dynamic);

			mVertexBuffer = BufferFactory::CreateVertexBuffer(maxVerts * sizeof(T), EBufferLifetime::Dynamic);

			mVertexBuffer.As<VertexBuffer>()->SetLayout(T::GetLayout());

			mVertexArray = VertexArrayFactory::Create({mVertexBuffer}, mIndexBuffer);
		}

		T *PushVertex() { return &mVertexCPU[mVertexCount++]; }

		uint32_t *PushIndex() { return &mIndexCPU[mIndexCount++]; }

		// sets the index and vertex buffer data
		void Upload()
		{
			mVertexBuffer.As<VertexBuffer>()->SetData(mVertexCPU.data(), mVertexCount * sizeof(T));

			if (mUseIndexBuffer)
				mIndexBuffer.As<IndexBuffer>()->SetData(mIndexCPU.data(), mIndexCount * sizeof(uint32_t));
		}

		void Reset()
		{
			mIndexCount = 0;
			mVertexCount = 0;
		}

		VertexArrayPtr GetVAO() const { return mVertexArray; }
		uint32_t GetIndexCount() const { return mIndexCount; }
		uint32_t GetVertexCount() const { return mVertexCount; }

	private:
		bool mUseIndexBuffer{false};

		std::vector<T> mVertexCPU{};
		std::vector<uint32_t> mIndexCPU{};

		uint32_t mIndexCount = 0;
		uint32_t mVertexCount = 0;

		IndexBufferPtr mIndexBuffer;
		VertexBufferPtr mVertexBuffer;
		VertexArrayPtr mVertexArray;
	};
} // namespace BHive