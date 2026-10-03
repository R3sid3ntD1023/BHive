#pragma once

#include "Buffer.h"
#include <memory>
#include <stdexcept>
#include <type_traits>

namespace BHive
{
	template <typename T>
	struct BufferArena
	{
		BufferArena(size_t size = 1024 * 1024) { mBuffer.Allocate(size); }

		std::byte *Allocate(size_t size, size_t alignment)
			requires std::is_same_v<T, std::byte>
		{
			if (alignment == 0 || (alignment & (alignment - 1)) != 0)
				throw std::invalid_argument("BufferArena alignment must be a power of two");

			void *aligned = reinterpret_cast<std::byte *>(mBuffer.GetData()) + mHead;
			size_t space = mBuffer.GetSize() - mHead;
			if (!std::align(alignment, size, aligned, space))
			{
				const size_t required = mHead + size + alignment - 1;
				const size_t capacity = mBuffer.GetSize();
				const size_t newCapacity = std::max(required, capacity ? capacity * 2 : size + alignment);
				MemoryBlock<T> expanded(newCapacity);
				if (mHead)
					std::memcpy(expanded.GetData(), mBuffer.GetData(), mHead);
				mBuffer = std::move(expanded);

				aligned = reinterpret_cast<std::byte *>(mBuffer.GetData()) + mHead;
				space = mBuffer.GetSize() - mHead;
				const bool alignedSuccessfully = std::align(alignment, size, aligned, space) != nullptr;
				if (!alignedSuccessfully)
					throw std::bad_alloc();
			}

			auto *result = static_cast<std::byte *>(aligned);
			mHead = static_cast<size_t>(result - reinterpret_cast<std::byte *>(mBuffer.GetData())) + size;
			return result;
		}

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
		const MemoryBlock<T> &GetBuffer() const { return mBuffer; }

	private:
		MemoryBlock<T> mBuffer;
		size_t mHead = 0;
	};
} // namespace BHive