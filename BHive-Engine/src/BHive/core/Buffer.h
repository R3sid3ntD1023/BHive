#pragma once

#include <stdint.h>

namespace BHive
{
	template <typename T>
	struct MemoryBlock
	{
		static constexpr size_t ValueSize = sizeof(T);

	private:
		T *mData = nullptr;
		uint64_t mSize = 0;

	public:
		MemoryBlock() = default;

		~MemoryBlock() { Release(); }

		MemoryBlock(const MemoryBlock &other)
		{
			Allocate(other.mSize);
			memcpy_s(mData, other.mSize, other.mData, mSize);
		}

		MemoryBlock(MemoryBlock &&other) noexcept
			: mData(other.mData),
			  mSize(other.mSize)
		{
			other.mData = nullptr;
			other.mSize = 0;
		}

		explicit MemoryBlock(uint64_t size) { Allocate(size); }

		explicit MemoryBlock(const void *data, uint64_t size) { Allocate(data, size); }

		void Allocate(const void *data, uint64_t size)
		{
			Allocate(size);
			memcpy_s(mData, mSize, data, size);
		}

		void Allocate(uint64_t size)
		{
			Release();

			mSize = size;
			mData = new T[size + 1];
			mData[size] = 0;
		}

		size_t GetSize() const { return mSize; }

		T *GetData() const { return mData; }

		T *GetData() { return mData; }

		operator T *() { return GetData(); }

		operator T *() const { return GetData(); }

		operator void *() const { return mData; }

		MemoryBlock &operator=(const MemoryBlock &rhs)
		{
			if (this == &rhs)
				return *this;

			Allocate(rhs.mSize);
			memcpy_s(mData, mSize, rhs.mData, rhs.mSize);
			return *this;
		}

		MemoryBlock &operator=(MemoryBlock &&rhs) noexcept
		{
			if (this == &rhs)
				return *this;

			Release();

			mData = rhs.mData;
			mSize = rhs.mSize;

			rhs.mData = nullptr;
			rhs.mSize = 0;

			return *this;
		}

		operator bool() const { return mData != nullptr && mSize != 0; }

	private:
		void Release()
		{
			if (mData)
				delete[] mData;

			mSize = 0;
			mData = nullptr;
		}
	};

	using ByteBuffer = MemoryBlock<uint8_t>;

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

		uint8_t *Data(size_t head) const { return mBuffer.GetData() + head; }

		size_t GetUsedSize() const { return mHead; }

		size_t GetCapacity() const { return mBuffer.GetSize(); }

	private:
		ByteBuffer mBuffer;
		size_t mHead = 0;
	};
} // namespace BHive