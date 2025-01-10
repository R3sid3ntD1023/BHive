#pragma once

#include <stdint.h>

namespace BHive
{
    template<typename T>
    struct TBuffer
    {
        T* mData = nullptr;
        uint64_t mSize = 0;

        TBuffer() = default;
        TBuffer(const TBuffer&) = default;

        TBuffer(uint64_t size)
        {
            Allocate(size);
        }

        TBuffer(T* data, uint64_t size)
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
            delete[] mData;
            mSize = 0;
            mData = nullptr;
        }

        static TBuffer Copy(TBuffer other)
        {
            TBuffer result(other.mSize);
            memcpy_s(result.mData, result.mSize, other.mData, other.mSize);
            return result;
        }


        operator bool() const
        {
            return mData != nullptr && mSize != 0;
        }
    };

    struct Buffer : public TBuffer<uint8_t>
	{
		Buffer() = default;
		Buffer(const Buffer& b)
			: TBuffer<uint8_t>(b)
            {}

		Buffer(uint64_t size)
			: TBuffer(size)
		{
		}

		template <typename T>
		T *As()
		{
			return (T *)mData;
		}
	};

    struct ScopedBuffer
    {
        ScopedBuffer(Buffer buffer)
            :mBuffer(buffer)
        {}

        ScopedBuffer(uint64_t size)
            :mBuffer(size)
        {}

        ~ScopedBuffer()
        {
            mBuffer.Release();
        }

        uint8_t* Data() { return mBuffer.mData; }
        uint64_t Size() { return mBuffer.mSize; }

        template<typename T>
        T* As()
        {
            return mBuffer.As<T>();
        }

        operator bool() const { return mBuffer; }

    private:
        Buffer mBuffer;
    };
}