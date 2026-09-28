#pragma once

#include "Buffer.h"

namespace BHive
{
	template <typename T>
	struct BufferArena
	{
		BufferArena(size_t size = 1024 * 1024) { mBuffer.Allocate(size); }

		size_t Push(const void *data, size_t size)
		{
			ASSERT(mHead + size <= mBuffer.GetSize());

			size_t offset = mHead;
			memcpy_s(mBuffer.GetData() + offset, mBuffer.GetSize() - offset, data, size);
			mHead += size;
			return offset;
		}

		void Reset() { mHead = 0; }

		T *Data(size_t head) const { return mBuffer.GetData() + head; }

		size_t GetUsedSize() const { return mHead; }

		size_t GetCapacity() const { return mBuffer.GetSize(); }

		MemoryBlock<T> &GetBuffer() { return mBuffer; }

	private:
		MemoryBlock<T> mBuffer;
		size_t mHead = 0;
	};
} // namespace BHive