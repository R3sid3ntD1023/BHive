#pragma once

#include "BufferBase.h"
#include "BufferLayout.h"
#include "core/EnumFlags.h"

namespace BHive
{

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
	};

	class BHIVE_API VertexBuffer : public BufferBase
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void SetLayout(const BufferLayout &layout) = 0;

		virtual const BufferLayout &GetLayout() const = 0;
	};

	class BHIVE_API GeneralBuffer : public BufferBase
	{
	public:
		virtual ~GeneralBuffer() = default;

		virtual void BindAtBindingPoint(uint32_t binding) = 0;
	};

} // namespace BHive