#pragma once

#include <stdint.h>

namespace BHive
{
	template <typename T>
	struct TBuffer
	{
	private:
		T *mData = nullptr;
		uint64_t mSize = 0;

	public:
		TBuffer() = default;
		TBuffer(const TBuffer &) = default;

		explicit TBuffer(uint64_t size) { Allocate(size); }

		explicit TBuffer(const void *data, uint64_t size) { Allocate(data, size); }

		void Allocate(const void *data, uint64_t size)
		{
			Allocate(size);
			memcpy_s(mData, mSize, data, size);
		}

		void Allocate(uint64_t size)
		{
			mSize = size;
			mData = new T[size + 1];
			mData[size] = 0;
		}

		void Release()
		{
			if (mData)
				delete[] mData;

			mSize = 0;
			mData = nullptr;
		}

		size_t GetSize() const { return mSize; }

		T *GetData() const { return mData; }

		T *GetData() { return mData; }

		operator T *() { return GetData(); }

		operator T *() const { return GetData(); }

		operator void *() const { return mData; }

		static TBuffer Copy(TBuffer other)
		{
			TBuffer result(other.mSize);
			memcpy_s(result.mData, result.mSize, other.mData, other.mSize);
			return result;
		}

		operator bool() const { return mData != nullptr && mSize != 0; }
	};

	struct Buffer : public TBuffer<uint8_t>
	{
		Buffer() = default;
		Buffer(const Buffer &b)
			: TBuffer<uint8_t>(b)
		{
		}

		explicit Buffer(uint64_t size)
			: TBuffer(size)
		{
		}

		explicit Buffer(const void *data, uint64_t size)
			: TBuffer(data, size)
		{
		}

		template <typename T>
		T *As() const
		{
			return (T *)GetData();
		}
	};

	struct ScopedBuffer
	{
		ScopedBuffer(Buffer buffer)
			: mBuffer(buffer)
		{
		}

		ScopedBuffer(uint64_t size)
			: mBuffer(size)
		{
		}

		~ScopedBuffer() { mBuffer.Release(); }

		operator bool() const { return mBuffer; }

	private:
		Buffer mBuffer;
	};
} // namespace BHive