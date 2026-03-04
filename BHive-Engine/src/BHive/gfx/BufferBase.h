#pragma once

#include "core/Core.h"
#include "NativeHandle.h"

namespace BHive
{
	

	class BHIVE_API BufferBase
	{
	public:
		enum Type
		{
			IndexBuffer,
			VertexBuffer,
			UniformBuffer,
			StorageBuffer
		};

		virtual ~BufferBase() = default;

		virtual void SetData(const void *data, size_t size, uint32_t offset = 0) = 0;

		virtual NativeHandle GetNativeHandle(uint32_t frame = 0) const = 0;

		virtual Type GetType() const = 0;
	};
} // namespace BHive