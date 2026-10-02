#pragma once

#include "BufferBase.h"
#include "BufferLayout.h"
#include "gfx/Enumerations.h"

namespace BHive
{

	struct FBufferCreateInfo
	{
		size_t ByteSize;
		EBufferType Type;
		EBufferLifetime LifeTime = EBufferLifetime::Static;
		void *Data = nullptr;
		uint32_t ElementCount = 0; // only used for index buffers
	};

	class BHIVE_API IndexBuffer : public BufferBase
	{
	public:
		IndexBuffer(size_t _bufSize, EBufferLifetime lifeTime, const uint32_t *data, uint32_t count);

		~IndexBuffer();

		void SetData(const void *data, size_t size, uint32_t offset = 0) override;

		void Clear() override;

		uint32_t GetCount() const { return mCount; }

		int32_t GetBufferID() const { return mBufferID; }

	private:
		uint32_t mCount = 0;
		int32_t mBufferID = -1;
	};

	class BHIVE_API VertexBuffer : public BufferBase
	{
	public:
		VertexBuffer(size_t bufSize, EBufferLifetime lifeTime, const void *data, size_t size);

		~VertexBuffer();

		void SetData(const void *data, size_t size, uint32_t offset = 0) override;

		void Clear() override;

		void SetLayout(const BufferLayout &layout) { mLayout = layout; };

		const BufferLayout &GetLayout() const { return mLayout; };

		int32_t GetBufferID() const { return mBufferID; }

	private:
		int32_t mBufferID = -1;
		BufferLayout mLayout;
	};

	class BHIVE_API GeneralBuffer : public BufferBase
	{
	public:
		GeneralBuffer(size_t _bufSize, EBufferType type, EBufferLifetime lifeTime, const void *data, size_t size);

		~GeneralBuffer();

		void SetData(const void *data, size_t size, uint32_t offset = 0) override;

		void Clear() override;

		int32_t GetBufferID() const { return mBufferID; }

	private:
		int32_t mBufferID = -1;
	};

} // namespace BHive