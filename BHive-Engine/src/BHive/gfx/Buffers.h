#pragma once

#include "BufferBase.h"
#include "BufferLayout.h"
#include "core/EnumFlags.h"

namespace BHive
{
	enum class EBufferType : uint8_t
	{
		Undefined = 0,
		UniformBuffer = BIT(0),
		StorageBuffer = BIT(1),
		IndirectBuffer = BIT(2)
	};

	ENABLE_BITMASK_OPERATORS(EBufferType)

	enum class EBufferLifetime
	{
		Static,
		Dynamic
	};

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
		virtual ~IndexBuffer() = default;

		virtual uint32_t GetCount() const = 0;

		static Ref<IndexBuffer> Create(const uint32_t count, EBufferLifetime lifetime = EBufferLifetime::Dynamic, const uint32_t *data = nullptr);
	};

	class BHIVE_API VertexBuffer : public BufferBase
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void SetLayout(const BufferLayout &layout) = 0;

		virtual const BufferLayout &GetLayout() const = 0;

		static Ref<VertexBuffer> Create(const uint64_t size, EBufferLifetime lifetime = EBufferLifetime::Dynamic, const void *data = nullptr);
	};

	class BHIVE_API GeneralBuffer : public BufferBase
	{
	public:
		virtual ~GeneralBuffer() = default;

		virtual void BindAtBindingPoint(uint32_t binding) = 0;

		static Ref<GeneralBuffer> Create(size_t size, EBufferType type, EBufferLifetime lifetime = EBufferLifetime::Dynamic, const void *data = nullptr);
	};

} // namespace BHive