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

		MemoryBlock(const MemoryBlock &other) { Allocate(other.mData, other.mSize); }

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
			memset(mData, 0, size);
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

} // namespace BHive