#pragma once

#include <stdint.h>

namespace BHive
{
    struct Buffer
    {
        uint8_t* mData;
        uint64_t mSize;

        Buffer() = default;
        Buffer(const Buffer&) = default;

        Buffer(uint64_t size)
        {
            Allocate(size);
        }

        Buffer(uint8_t* data, uint64_t size)
        {
            Allocate(size);
            memcpy(mData, data, size);
        }

        void Allocate(uint64_t size)
        {
            mSize = size;
            mData = new uint8_t[size + 1];
            mData[size] = 0;
        }

        void Release()
        {
            delete[] mData;
            mSize = 0;
            mData = nullptr;
        }

        static Buffer Copy(Buffer other)
        {
            Buffer result(other.mSize);
            memcpy(result.mData, other.mData, other.mSize);
            return result;
        }

        template<typename T>
        T* As()
        {
            return (T*)mData;
        }

        operator bool() const
        {
            return (bool)mData;
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